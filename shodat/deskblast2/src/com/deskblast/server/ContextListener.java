package com.deskblast.server;

import javax.servlet.ServletContextEvent;
import javax.servlet.ServletContextListener;

public class ContextListener implements ServletContextListener {

	public void contextInitialized(ServletContextEvent event) {
		BlastRegistrar blastRegistrar = new BlastRegistrar();
		event.getServletContext().setAttribute(BlastRegistrar.NAME, 
				blastRegistrar);
	}

	public void contextDestroyed(ServletContextEvent event) {
		BlastRegistrar.getBlastRegistrar(event.getServletContext()).cleanup();
	}

}
