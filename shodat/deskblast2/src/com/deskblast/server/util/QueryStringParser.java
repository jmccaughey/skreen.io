package com.deskblast.server.util;

// TODO replace with apache library ?
public class QueryStringParser {

	private String query;

	public QueryStringParser(String query) {
		super();
		this.query = query;
	}

	public String getParameter(String name){
		int start = name.length() + 1 + this.query.indexOf(name + "=");
		int end = this.query.indexOf("&", start);
		if(end == -1){
			return this.query.substring(start);
		}
		return this.query.substring(start, end);
	}
	
	public static void main(String[] args){
		String query = "?dogName=frisky&age=20";
		QueryStringParser qsp = new QueryStringParser(query);
		System.out.println(qsp.getParameter("dogName"));
		System.out.println(qsp.getParameter("age"));		
	}
	
	
}
