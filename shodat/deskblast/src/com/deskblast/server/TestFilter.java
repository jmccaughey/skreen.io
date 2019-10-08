package com.deskblast.server;

import java.io.IOException;

import javax.servlet.Filter;
import javax.servlet.FilterChain;
import javax.servlet.FilterConfig;
import javax.servlet.RequestDispatcher;
import javax.servlet.ServletException;
import javax.servlet.ServletRequest;
import javax.servlet.ServletResponse;
import javax.servlet.http.HttpServletRequest;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

public class TestFilter implements Filter {

	private final Log log = LogFactory.getLog(getClass());
	
	@Override
	public void destroy() {
		// TODO Auto-generated method stub

	}

	@Override
	public void doFilter(ServletRequest arg0, ServletResponse arg1,
			FilterChain arg2) throws IOException, ServletException {
		log.info("new test filter ");
		HttpServletRequest req = (HttpServletRequest)arg0;
		
		Long joinCode = null;
		log.info("path info: " + req.getPathInfo());			
		log.info("uri: " + req.getRequestURI());
		try{
			String document = req.getRequestURI().substring(1);
			joinCode = Long.parseLong(document);
			log.info("test filter: got join code " + joinCode);
			//
		}catch(Exception e){
			arg2.doFilter(arg0, arg1);
		}
		//arg2.doFilter(arg0, arg1);		
		
		//RequestDispatcher requestDispatcher = req.getRequestDispatcher();
		//log.info(requestDispatcher);
		
	}

	@Override
	public void init(FilterConfig arg0) throws ServletException {
		// TODO Auto-generated method stub

	}

}
