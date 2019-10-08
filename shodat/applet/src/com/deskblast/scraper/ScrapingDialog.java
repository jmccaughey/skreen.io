package com.deskblast.scraper;

import java.awt.Button;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.FlowLayout;
import java.awt.Frame;
import java.awt.Graphics;
import java.awt.Label;
import java.awt.Point;
import java.awt.Rectangle;
import java.awt.Toolkit;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.io.IOException;


public class ScrapingDialog extends Frame{
	
	@Override
	public void paint(Graphics g) {
		super.paint(g);
		Rectangle rect = this.getBounds();
		g.drawLine(0, rect.height-1, rect.width, rect.height-1);		
		g.drawLine(0, 0, 0, rect.height-1);
		g.drawLine(rect.width-1, 0, rect.width-1, rect.height-1);
		g.drawLine(0, 0, rect.width-1, 0);
	}

	//private UserStoppedListener userStoppedListener;
	private Button btnStop;
	private static final String STOP_ACTION_COMMAND = "STOP_ACTION_COMMAND"; 
    private static final String STOP_BUTTON_TEXT = "Stop";
    private static final String SHOING_TEXT = "shoing";
    private static final String PROCESS_NAME = "shodat";
	//private Point offset = null;
    
    private boolean dragging;
    private Point clickPoint;
    
	/**
	 * @param userStoppedListener
	 */
	public ScrapingDialog(final UserStoppedListener userStoppedListener){
		super(PROCESS_NAME);
		setResizable(false);
        setLayout(new FlowLayout());
		setAlwaysOnTop(true);
		setUndecorated(true);
		Label label = new Label(SHOING_TEXT, Label.CENTER);		
		label.setEnabled(false);
		//label.setFont(label.getFont())
		//label.setBackground(Color.GREEN);
		
        add(label);
		btnStop = new Button(STOP_BUTTON_TEXT);    
        btnStop.setActionCommand(STOP_ACTION_COMMAND);
        add(btnStop);
        btnStop.addActionListener(new ActionListener(){
            @Override
            public void actionPerformed(ActionEvent e) {
                if(e.getActionCommand().equals(STOP_ACTION_COMMAND)){
                    System.out.println("got stop action");    
                    Component source = (Component)e.getSource();
                    System.out.println(source.getParent());
					userStoppedListener.userStopped();
					setVisible(false);
					dispose();
                }
            }}
        );
        addMouseListener(new MouseAdapter() {
			public void mousePressed(MouseEvent e) {
				clickPoint = e.getPoint();
				System.out.println(e);
				dragging = true;
			}
			public void mouseReleased(MouseEvent e) {
				dragging = false;
				System.out.println(e);
			}
		});
        try{
	        addMouseMotionListener(new MouseAdapter() {
				public void mouseDragged(MouseEvent e) {
					if(dragging){
						moveTo(e.getPoint());
					}
				}
	        });
        }catch(Throwable t){
        	t.printStackTrace();
        }
        setIconImage(Toolkit.getDefaultToolkit().getImage(this.getClass().getResource("/32_32.gif")));
        setVisible(true);
        pack();
        Dimension screen = Toolkit.getDefaultToolkit().getScreenSize();
        Dimension dim = this.getSize();
        int y = -4;
        if(System.getProperty("os.name").toLowerCase().startsWith("mac")){
        	y = 20;
        }
        setLocation((screen.width / 2) - (dim.width / 2), y);
	}
	
	private void moveTo(Point p){
		int offsetX = p.x - clickPoint.x;
		int offsetY = p.y - clickPoint.y;
		setLocation( getLocation().x + offsetX, getLocation().y + offsetY );
	}

	public static void main(String[] args){
        //Frame frame = new Frame("parent");
        //frame.setSize(400, 300);
        ScrapingDialog sd = new ScrapingDialog(new UserStoppedListener() {
			public void userStopped() {
				System.out.println("handling userStopped event");
			}
		}); 
        sd.setVisible(true);
        try {
			System.in.read();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		sd.dispose();
	}
	
}