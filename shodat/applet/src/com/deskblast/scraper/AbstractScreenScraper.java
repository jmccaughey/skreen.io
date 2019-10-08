package com.deskblast.scraper;

import java.awt.Dimension;
import java.awt.Frame;
import java.awt.Point;
import java.awt.Rectangle;
import java.io.File;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.Vector;

import com.deskblast.scrape.protocol.AbstractTileInfo;
import com.deskblast.scrape.protocol.BackReferenceScreenScrape;
import com.deskblast.scrape.protocol.CompressedScreenScrape;
import com.deskblast.scrape.protocol.ScreenScrapeData;

public abstract class AbstractScreenScraper
    implements
        Runnable
        {
	static final int GIF_THRESHOLD = 256;
	static final int PNG_THRESHOLD = 1024;
	static final int TILE_WIDTH = 256;
	static final int TILE_HEIGHT = 128;
	static final int MAX_MERGED_TILES = 5;
	
	
    public static AbstractScreenScraper getInstance()
    throws Exception{
        String strVendor = System.getProperty("java.vendor");
        //String strVersion
        Class clazz = null;
        AbstractScreenScraper scraper = null;
        //try{
            if(strVendor.indexOf("Microsoft") > -1){
//                if(Class.forName(
//                    "com.ms.security.PolicyEngine") != null) {
//                    com.ms.security.PolicyEngine.assertPermission(
//                        com.ms.security.PermissionID.SYSTEM);
//                }
                clazz = Class.forName("com.sts.webmeet.content.client.appshare.Win32ScreenScraper");
            }else{
                clazz = Class.forName("com.deskblast.scraper.Java2ScreenScraper");
            }
            if(null != clazz){
                scraper = (AbstractScreenScraper)(clazz.newInstance());
            }
        //}catch(Exception e){
        //  e.printStackTrace();
        //}
        return scraper;
    }

    public abstract int[] grabPixels(int[] pixels, Rectangle rect, Rectangle rectObscure)
    throws Exception;

    public void startScraping(ScreenScraperContext context, Frame parent){

        this.context = context;

        thread = new Thread(this, getClass().getName());
        bContinue = true;
        try{
        	this.scrapingDialog = new ScrapingDialog(new UserStoppedListener() {
				@Override
				public void userStopped() {
					userStopped = true;
					bContinue = false;
				}
			});        	
            thread.start();
        }catch(Exception e){
            e.printStackTrace();
        }
    }

    public void stopScraping(){
        bContinue = false;
        try{
            thread.join();
            thread = null;
        }catch(Exception e){
            e.printStackTrace();
        }
    }

    public void scrapeRequested(){
        bScrapeRequested = true;
    }
    public void run(){
        try{
            // FIXME todo xxx: if connection is dropped, scrape request
            // message might be missed and cycle is broken
            //
            bScrapeRequested = true;
            while(bContinue){
                if(bScrapeRequested){
                    bScrapeRequested = false;
                    doScrape();                   
                }
                try{
                    Thread.sleep(this.context.getScrapeInterval());
                }catch(Exception e){
                    e.printStackTrace();
                    break;
                }
            }
            if(userStopped){
            	userStopped = false;
            	context.userStopped();
            }
        }catch(Exception e){
            e.printStackTrace();
        }
    }

    public byte[] compressPixels(int[] pixels, int iWidth, int iHeight){
    	byte[] baRet = doJpegCompression(pixels, iWidth, iHeight);
    	return baRet;
    }
        
    public abstract byte[] doJpegCompression(
    		int[] pixels, int iWidth, int iHeight);
    
    public abstract byte[] doGifCompression(
    		int[] pixels, int iWidth, int iHeight);
    
    public abstract byte[] doPngCompression(
    		int[] iaPixels, int width, int height);

    public abstract Point getMousePosition();
    
    private void doScrape()
    throws Exception{
        Rectangle fullScrapeRect = context.getScrapeRegion();
        boolean bDoKeyFrame = context.isKeyFrameDue();        
        if(null == currentPixels 
        		|| null == this.lastRect
        		|| !this.lastRect.equals(fullScrapeRect)){
            
        	currentPixels = new int[fullScrapeRect.width * fullScrapeRect.height];
        	previousPixels = new int[fullScrapeRect.width * fullScrapeRect.height];
        	bDoKeyFrame = true; 
        	this.lastRect = fullScrapeRect;
        	this.scrapeRects = TileMaker.makeTiles(
        			new Dimension(fullScrapeRect.width, fullScrapeRect.height), 
        			TILE_WIDTH, TILE_HEIGHT
        	);
        }           
        Point point = getMousePosition();
        if(context.getScrapeSource() != null){
        	context.getScrapeSource().getNextScrape(
        			this.currentPixels, fullScrapeRect);
        }else{
        	this.grabPixels(this.currentPixels, fullScrapeRect, this.scrapingDialog.getBounds());
    	}
        // OK now we have the raw sub rects for the current
        // screen snapshot.  If no keyframe is required already,
        // and previous sub rects exist, compare them 
        
        Rectangle[] changedRects = null;
        AbstractTileInfo[] aRectsToSend = null;
        
        if(bDoKeyFrame){        	
            changedRects = this.scrapeRects;
        }else{
            changedRects = this.getChangedRects();
        }
       	aRectsToSend = compressRawScrapes(changedRects, fullScrapeRect.width);
       		
       	int[] tmp = this.previousPixels;
       	this.previousPixels = this.currentPixels;
       	this.currentPixels = tmp;
        boolean sendingKeyFrame = this.scrapeRects.length == aRectsToSend.length;
        if(sendingKeyFrame){
            System.out.println("sending key frame");
        }
        if(lastPointSent == null || (!lastPointSent.equals(point)) || aRectsToSend.length > 0){
            
        }
        // TODO: THROTTLE sends if no change?
        context.dataReady(
            new ScreenScrapeData(aRectsToSend, fullScrapeRect, point, sendingKeyFrame)
        );
        lastPointSent = point;
    }

    // TODO: parallelize this?
    private Rectangle[] getChangedRects(){
        Vector vectChanged = new Vector();
        for(int i = 0; i < this.scrapeRects.length; i++){            
            if(ArrayChangeChecker.dataChanged(this.scrapeRects[i],
                    this.previousPixels, 
                    this.currentPixels, context.getScrapeRegion().width)){                
                vectChanged.addElement(this.scrapeRects[i]);
            }
        }       
        return (Rectangle[])vectChanged.toArray(new Rectangle[0]);
    }
  
    private AbstractTileInfo[] compressRawScrapes(
    		Rectangle[] rects, int scansize //RawScrape[] scrapes, MemoryImageSource mis
    		)
    throws Exception{
    	List<AbstractTileInfo> atis = new ArrayList<AbstractTileInfo>();
    	// before merging, check for rects that have already been sent
    	// in other words, hash each rect and check for it in the map
    	// of hashes to containing sprites name & offsets
    	// if it has already been sent, send a backreferencescrape
    	// & remove it from rects to be merged 
    	List<Rectangle> newRects = new ArrayList<Rectangle>();
    	Map<Rectangle, String> rectHashes = new HashMap<Rectangle, String>();
    	for(int i = 0; i < rects.length; i++){
    		String hash = MD5Hasher.doMD5Hash(rects[i], this.currentPixels, scansize);
    		if(this.subTiles.containsKey(hash)){
    			SubtileInfo sti = this.subTiles.get(hash);
    			BackReferenceScreenScrape brss = new BackReferenceScreenScrape(
						rects[i].x, rects[i].y, rects[i].width, rects[i].height, sti.getHash(), 
						sti.getX(), sti.getY());
				atis.add(brss);
				System.out.println("found subtile hash");
    		}else{
    			newRects.add(rects[i]);
    			rectHashes.put(rects[i], hash);
    		}
    	}
    	Rectangle[] merged = RectMerger.mergeRectangles(newRects.toArray(new Rectangle[0]), 
    			TILE_WIDTH * TILE_HEIGHT * MAX_MERGED_TILES);
    	System.out.println(newRects.size() + " rects merged to " + merged.length);
    	for(int i = 0; i < merged.length; i++){
    		String hash = MD5Hasher.doMD5Hash(merged[i], this.currentPixels, scansize);
    		if(this.sentScrapeHashes.contains(hash)){
    			for(Rectangle rect : newRects){
    				if(merged[i].contains(rect)){
    					BackReferenceScreenScrape brss = new BackReferenceScreenScrape(
    							rect.x, rect.y, rect.width, rect.height, hash, 
    							rect.x - merged[i].x, rect.y - merged[i].y);
    					atis.add(brss);
    				}
    			}
    		}else{
    			this.sentScrapeHashes.add(hash);
    			int[] rgb = PixelUtil.getSubtilePixels(merged[i], this.currentPixels, scansize);
    			byte[] baCompressed = doGifCompression(rgb, merged[i].width, merged[i].height);
    			if(null == baCompressed){
    				baCompressed = doJpegCompression(rgb, merged[i].width, merged[i].height);
    			}
    			boolean spriteSent = false;
    			for(Rectangle rect : newRects){
    				if(merged[i].contains(rect)){
    					if(!spriteSent){
	    					CompressedScreenScrape css = new CompressedScreenScrape(baCompressed,
	    							rect.x, rect.y, rect.width, rect.height, hash, 
	    							rect.x - merged[i].x, rect.y - merged[i].y);
	    					atis.add(css);
	    					spriteSent = true;
    					}else{
        					BackReferenceScreenScrape brss = new BackReferenceScreenScrape(
        							rect.x, rect.y, rect.width, rect.height, hash, 
        							rect.x - merged[i].x, rect.y - merged[i].y);
        					atis.add(brss);
    					}
    					String tileHash = rectHashes.get(rect);
    					this.subTiles.put(tileHash, new SubtileInfo(hash, 
    							rect.x - merged[i].x, rect.y - merged[i].y));
    				}
    			}
    		}
    	}
    	return atis.toArray(new AbstractTileInfo[0]);
    	
    	// state is two hashes: one of uploaded sprites
    	// the other of child tiles 
    	// child tiles key is tile hash, value is spriteinfo (hash and offsets)
    	
    	// hash each non-merged tile
    	// the map key is the hash
    	// the info is the containing image (sprite) hash
    	// plus the sprite offsets
    }
   
    private AbstractTileInfo makeTileInfo(
    		Rectangle rect
    )throws Exception{
    	return this.makeCompressedScreenScrapeRect(rect, this.currentPixels);
    }
        
    private AbstractTileInfo makeCompressedScreenScrapeRect(
      		Rectangle rect,
    		int[] iaPixels
    )throws Exception{
    	byte[] ba = null;
    	byte[] baJpeg = null;
    	byte[] baLossless = null;
    	String hash = MD5Hasher.doMD5Hash(rect, iaPixels, 
    			this.context.getScrapeRegion().width);
    	if(this.sentScrapeHashes.contains(hash)){
		    return new BackReferenceScreenScrape(rect.x, rect.y, 
		    		rect.width, rect.height, hash, 0, 0); 
		    // TODO: actually merge rects and pass real sprite offsets
		}
		this.sentScrapeHashes.add(hash);
   		baLossless = this.doGifCompression(iaPixels, rect.width, rect.height);
    	if(baLossless == null){
    		baJpeg = GZipCompressor.compress(
					this.doJpegCompression(iaPixels, rect.width, rect.height));
    	}
    	ba = baLossless != null ? baLossless : baJpeg;
		return new CompressedScreenScrape(ba, rect.x, rect.y, 
				rect.width, rect.height, hash, 0, 0);
    }
    
	private int countColorsUpToN(int[] iaPixels, int nColors) {
    	Set<Integer> colors = new HashSet<Integer>(nColors+1);
    	for(int i = 0; i < iaPixels.length; i++){
    		if(colors.size() == nColors){
    			return nColors;
    		}
    		colors.add(iaPixels[i]);
    	}
    	return colors.size();    	
	}
    
    // following found here:
    // http://www.anyexample.com/programming/java/java_simple_class_to_compute_md5_hash.xml
    private static String convertToHex(byte[] data) {
        StringBuffer buf = new StringBuffer();
        for (int i = 0; i < data.length; i++) {
            int halfbyte = (data[i] >>> 4) & 0x0F;
            int two_halfs = 0;
            do {
                if ((0 <= halfbyte) && (halfbyte <= 9))
                    buf.append((char) ('0' + halfbyte));
                else
                    buf.append((char) ('a' + (halfbyte - 10)));
                halfbyte = data[i] & 0x0F;
            } while(two_halfs++ < 1);
        }
        return buf.toString();
    }
    
    private Rectangle[] scrapeRects;
    int[] currentPixels;
    int[] previousPixels;
    private boolean bContinue;
    private Thread thread;
    private boolean bScrapeRequested;
    private ScreenScraperContext context;
    private Rectangle lastRect;
    private Set<String> sentScrapeHashes = new HashSet<String>();
    private Map<String, SubtileInfo> subTiles = new HashMap<String, SubtileInfo>();
    private Point lastPointSent;
    private ScrapingDialog scrapingDialog;
    private boolean userStopped;
    private int sequence;
    
    public static void main(String[] args) throws Exception{
        // TODO: assert that domain matches domain of origin
//    	int count = 256*256;
//    	int[] test = new int[count];
//    	int[] boundaries = 	{ Integer.MAX_VALUE, Integer.MIN_VALUE, -1, 0, 1, 99999};
//    	Random rand = new Random();
//    	for(int i = 0; i < count; i++){
//    		if(i < boundaries.length){
//    			test[i] = boundaries[i];
//    		}else{
//    			test[i] = rand.nextInt(); 
//    		}
//    	}
//    	long start = System.currentTimeMillis();
//    	String res1 = doMD5HashSlow(test);
//    	System.out.println(System.currentTimeMillis() - start);
//    	start = System.currentTimeMillis();
//    	String res2 = doMD5HashFast(test);
//    	System.out.println(System.currentTimeMillis() - start);
//    	System.out.println(res1);
//    	System.out.println(res2);
//    	System.out.println(res2.equals(res1));
//    	if(true){
//    		return;
//    	}
    	Frame frame = new Frame("Parent");
    	frame.setVisible(true);
    	File dir = new File(args[0]);
        ScrapeSender sender = new TestStatisticsScrapeSender(
                //new ServerInfo(false, "localhost", 8888, Long.valueOf(args[0]), Long.valueOf(args[1]),
				//		args[2], "http://localhost:8888")
        		null
        		,
                frame,
                new TestScrapeSource(dir)
        );
        //sender.
        System.in.read();
    }
}
/*
class ScrapeSender implements ScreenScraperContext{
    final Rectangle rect = new Rectangle(256*4, 256*3);
    int keyFrameInterval = 10;
    int scrapes = 0;
    AbstractScreenScraper scraper;
    String baseUrl = "http://localhost:8080/scrapes?";
    
    public ScrapeSender(Long blastId, Long blasterId, String key){
        baseUrl = baseUrl 
            + "blastId=" + blastId 
            + "&blasterId=" + blasterId
            + "&key=" + key;
        try {
            scraper = AbstractScreenScraper.getInstance();
        } catch (Exception e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
        scraper.startScraping(this);
    }
    public void dataReady(ScreenScrapeData data) {
        System.out.println(data);
        try {
            URL url = new URL(baseUrl);
            HttpURLConnection connection;
            connection = (HttpURLConnection) url.openConnection();
            connection.setDoOutput(true);
            connection.connect();
            DataOutputStream oos = new DataOutputStream(connection
                    .getOutputStream());
            data.writeToDataOututStream(oos);
            oos.flush();
            oos.close();
            int response = connection.getInputStream().read();
            if(response > 0){
                scraper.scrapeRequested();
                scrapes++;
            }
        } catch (Exception e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
    }
    public int getScrapeInterval() {
        return 400;
    }
    public Rectangle getScrapeRegion() {
        return rect;
    }
    public boolean isKeyFrameDue() {
        return (scrapes % keyFrameInterval) == 0;
    }    
}
*/