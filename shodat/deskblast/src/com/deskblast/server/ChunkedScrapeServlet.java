package com.deskblast.server;

import java.io.DataInputStream;
import java.io.IOException;

import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import com.deskblast.server.util.QueryStringParser;

public class ChunkedScrapeServlet extends HttpServlet {

	private final Log log = LogFactory.getLog(getClass());

	protected void doPost(HttpServletRequest request,
			HttpServletResponse response) throws ServletException, IOException {
		//System.out.println("serving scrape post");
		DataInputStream dis = new DataInputStream(request.getInputStream());
		//System.out.println("content-length:" + request.getContentLength());
		try {			
			System.out.println("got posted screen scrape chunk");
			//System.out.println(scrape);
			// get request parms from the URL manually
			// otherwise getParameter looks at the URL and
			// the entire post body
			QueryStringParser queryStringParser = new QueryStringParser(request
					.getQueryString());
			int blastId = Integer.parseInt(queryStringParser
					.getParameter("blastId"));
			String key = queryStringParser.getParameter("key");
			int blasterId = Integer.parseInt(queryStringParser
					.getParameter("blasterId"));
			log.debug("handlng posted scrape");
			ContinueInfo continueInfo = BlastRegistrar
					.getBlastRegistrar(request).getBlast(blastId)
					.handleScreenScrapeChunk(dis, blasterId, key);
					//.handleScreenScrape(dis, blasterId, key);
			if( ! continueInfo.isContinue()){
				log.debug("writing 0");
				response.getOutputStream().write(0);
			}else if( ! continueInfo.isKeyframeRequest()){
				log.debug("writing 1");
				response.getOutputStream().write(1);
			}else{
				log.debug("writing 2");
				response.getOutputStream().write(2);
			}
		} catch (Throwable e) {
			e.printStackTrace();
			throw new ServletException("problem reading scrape", e);
		}finally{
			dis.close();
		}
	}
	/*
	@Override
	protected void doPut(HttpServletRequest request,
			HttpServletResponse response) throws ServletException, IOException {
		doPost(request, response);
	}	
	@Override
	protected void doGet(HttpServletRequest request,
			HttpServletResponse response) throws ServletException, IOException {
		log.debug("got image request: " + request.getRequestURL()
				+ request.getQueryString());
		
//		String now = System.currentTimeMillis() + "";
//		if(now.endsWith("1") || now.endsWith("3") || now.endsWith("5") || now.endsWith("7")){
//			try {
//				Thread.currentThread().sleep(new Random().nextInt(4000));
//			} catch (InterruptedException e) {
//				e.printStackTrace();
//			}
//		}
		
		int blastId = Integer.parseInt((String) request.getParameter("blastId"));
		String key = (String) request.getParameter("key");
		String name = (String) request.getParameter("name");
		int blasterId = Integer.parseInt((String) request.getParameter("blasterId"));
		byte[] scrape = BlastRegistrar.getBlastRegistrar(request).getBlast(
				blastId).getScrapeFromCache(name, blasterId, key);
		if(scrape[0] == 'G' && scrape[1] == 'I' && scrape[2] == 'F'){
			response.setContentType("image/gif");
		}else if(scrape[1] == 'P' && scrape[2] == 'N' && scrape[3] == 'G'){
			response.setContentType("image/png");
		}else{
			response.setContentType("image/jpeg");
			if(scrape[6] == 'J' && scrape[7] == 'F' && scrape[8] == 'I' && scrape[9] == 'F'){
				// uncompressed
			}else{
				response.setHeader("Content-Encoding", "gzip");
			}
		}
		response.setDateHeader("Expires", future);
		response.setContentLength(scrape.length);		
		IOUtils.write(scrape, response.getOutputStream());
	}
	
//	public static void main(String[] args) throws FileNotFoundException, IOException, ClassNotFoundException{
//		DataInputStream dis = new DataInputStream(new FileInputStream("c://scrapes//scrape.bin"));
//		ScreenScrapeData scrape = new ScreenScrapeData(dis);
//		System.out.println(scrape);
//	}
*/
}
