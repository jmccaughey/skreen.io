package com.deskblast.client;

import com.google.gwt.user.client.rpc.ServiceDefTarget;

public class UrlUtil {

	public static void oneUpService(Object service){
		ServiceDefTarget serviceDefTarget = (ServiceDefTarget)service;
		serviceDefTarget.setServiceEntryPoint(oneUpUrl(serviceDefTarget.getServiceEntryPoint()));
	}
	
	public static String oneUpUrl(String original){
		String[] parts = original.split("/");
		String result = "";
		for(int i = 0; i < parts.length; i++){
			if(parts[i].length() == 0){
				// skip empties
			}else if(i == 0){
				result += parts[i] + "/"; 
			}else if(i == parts.length -2){
				// drop context
			}else{
				result += "/" + parts[i];
			}
		}
		return result;
	}

}
