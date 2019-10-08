package com.deskblast.scraper;

import java.applet.Applet;
import java.awt.Container;
import java.awt.Frame;
import java.net.URL;
import java.util.Enumeration;

public class ScraperApplet extends Applet {

    private ScrapeSender scrapeSender;

    public void init() {
        super.init();
        System.out
                .println("applet code base: " + this.getCodeBase().toString());
        System.out.println("name: " + getParameter("name"));
        System.out.println("id: " + getParameter("id"));
        Enumeration enumer = getAppletContext().getApplets();
        while (enumer.hasMoreElements()) {
            Applet applet = (Applet) enumer.nextElement();
            System.out.println(applet.toString());
            System.out.println(applet.getName());
        }
        // String[] parms = getParameter("name").split("_");
        String blastId = getParameter("blastId");
        String blasterId = getParameter("blasterId");
        String key = getParameter("key");
        String context = getParameter("context");
        URL url = this.getCodeBase();
        System.out.println("applet code base: " + url.toString());
        Integer port = null;
        if (url.getPort() > 0) {
            port = Integer.valueOf(url.getPort());
        }
        //MsgBox msgBox = new MsgBox(findParentFrame(), "Deskblast",
        //        "Start showing your desktop?", true);
        //if (msgBox.isOk) {
            this.scrapeSender = new 
            	//ScrapeSender(
            	ChunkedScrapeSender(
                    new ServerInfo(url.toExternalForm().toLowerCase().startsWith("https://"), 
                    		url.getHost(), port, Long.valueOf(blastId),
							Long.valueOf(blasterId), key, context), 
                    findParentFrame(), null);
        //} else {
        //    System.out.println("user declined to show desktop");
        //}
    }

    private Frame findParentFrame() {
        Container c = this;
        while (c != null) {
            if (c instanceof Frame) {
                return (Frame) c;
            }
            c = c.getParent();
        }
        return (Frame) null;
    }

    public void destroy() {
        this.scrapeSender.stopScraping();
        super.destroy();
    }

}
