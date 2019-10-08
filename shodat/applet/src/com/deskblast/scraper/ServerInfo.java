package com.deskblast.scraper;

public class ServerInfo {
	public boolean ssl;
	public String host;
	public Integer port;
	public Long blastId;
	public Long blasterId;
	public String key;
	public String context;

	public ServerInfo(boolean ssl, String host, Integer port, Long blastId,
			Long blasterId, String key, String context) {
		this.ssl = ssl;
		this.host = host;
		this.port = port;
		this.blastId = blastId;
		this.blasterId = blasterId;
		this.key = key;
		this.context = context;
	}
}