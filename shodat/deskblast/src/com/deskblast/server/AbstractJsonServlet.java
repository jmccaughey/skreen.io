package com.deskblast.server;

import java.io.IOException;

import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.apache.commons.io.IOUtils;

import com.metaparadigm.jsonrpc.JSONSerializer;

public abstract class AbstractJsonServlet extends
		javax.servlet.http.HttpServlet {

	protected void doGet(HttpServletRequest httpServletRequest,
			HttpServletResponse httpServletResponse) throws ServletException, IOException {
		Object object = handleGetRequest(httpServletRequest);
		JSONSerializer jsonSerializer = new JSONSerializer();
		try {
			jsonSerializer.registerDefaultSerializers();
			String json = jsonSerializer.toJSON(object);
			if(json.length() > 5){
				System.out.println("writing: " + json);
			}
			httpServletResponse.addHeader("pragma", "No-Cache");
			httpServletResponse.setHeader("Cache-Control","no-cache");
			httpServletResponse.setDateHeader ("Expires", 0);
			httpServletResponse.setContentType("application/json");
			IOUtils.write(json, httpServletResponse.getOutputStream());
		} catch (Exception e) {
			e.printStackTrace();
			throw new ServletException("problem serializing json", e);
		}
	}

	protected void doPost(HttpServletRequest httpServletRequest,
			HttpServletResponse httpServletResponse) throws ServletException,
			IOException {
		JSONSerializer jsonSerializer = new JSONSerializer();
		String json = IOUtils.toString(httpServletRequest.getInputStream());
		System.out.println("got json: " + json);
		try {
			jsonSerializer.registerDefaultSerializers();
			Object object = handlePostRequest(httpServletRequest,
					jsonSerializer.fromJSON(json));
			if(object != null){
				String jsonOut = jsonSerializer.toJSON(object);
				System.out.println("writing: " + jsonOut);
				IOUtils.write(jsonOut, httpServletResponse.getOutputStream());
			}
		} catch (Exception e) {
			e.printStackTrace();
			throw new ServletException("problem serializing json", e);
		}
	}

	public abstract Object handleGetRequest(
			HttpServletRequest httpServletRequest) throws ServletException;

	public abstract Object handlePostRequest(
			HttpServletRequest httpServletRequest, Object object)
			throws ServletException;

}
