package com.deskblast.scraper;

import java.io.IOException;
import java.util.List;

import com.deskblast.scrape.protocol.Streamable;

public interface Uploader {
	
	public List<byte[]> upload(String url, Streamable[] streamables) throws IOException;

}
