package com.deskblast.server;

import java.io.Console;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import com.google.inject.Guice;
import com.google.inject.Injector;
import com.google.inject.servlet.GuiceServletContextListener;
import com.google.inject.servlet.ServletModule;

public class MyGuiceContextListener extends GuiceServletContextListener {

	private final Log log = LogFactory.getLog(getClass());
	
	@Override
	protected Injector getInjector() {
		log.info("getInjector");
		return Guice.createInjector(new ServletModule() {
			@Override
			protected void configureServlets() {
				log.info("configureServlets");
				//serveRegex("(.)*ajax(.)*").with(JoinServlet.class);
				serveRegex("(.)[0-9]{6}\\z").with(JoinServlet.class);
				//serve("*.txt").with(JoinServlet.class);
			}
		});
	}
	
	public static void main(String[] args){
		Pattern pattern = Pattern.compile(args[0]);
		Matcher matcher = pattern.matcher(args[1]);
		boolean found = false;
		//Console console = System.console();
        while (matcher.find()) {
            System.out.println("I found the text " + matcher.group()
            		+  " starting at " + matcher.start() + " and ending at index " + matcher.end());
            found = true;
        }
        if(!found){
            System.out.println("No match found.%n");
        }
		
		
	}
}
