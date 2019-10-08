package com.deskblast.scrape.protocol;

import java.io.DataOutputStream;
import java.io.IOException;

public interface Streamable {

	public void stream(DataOutputStream outputStream) throws IOException;
	
}
