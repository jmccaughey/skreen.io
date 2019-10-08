package com.deskblast.server;

import java.util.Collection;
import java.util.Collections;
import java.util.HashMap;
import java.util.Map;
import java.util.Random;
import java.util.UUID;

import javax.servlet.ServletContext;
import javax.servlet.http.HttpServletRequest;

import com.deskblast.client.RpcBlasterCredentials;
import com.deskblast.client.RpcBlasterInfo;

public class BlastRegistrar implements BlastEndedListener{

	static final String NAME = "blastRegistrar";

	private Map<Integer, RpcBlasterCredentials> pendingBlasts = Collections.synchronizedMap(
									new HashMap<Integer, RpcBlasterCredentials>());
	private Map<Integer, Blast> liveBlasts = Collections.synchronizedMap(
									new HashMap<Integer, Blast>());
	private Map<Integer, RpcBlasterCredentials> pendingBlasters = Collections.synchronizedMap(
									new HashMap<Integer, RpcBlasterCredentials>());
	
	public RpcBlasterCredentials createBlast(String ownerName) {
		Blast existingBlast = findBlastByOwnerName(ownerName);
		if(existingBlast != null){
			System.out.println("returning existing blast");
			return existingBlast.getOwnerCredentials();
		}
		Random random = new Random();
		RpcBlasterInfo rpcBlasterInfo = new RpcBlasterInfo(
					ownerName, Math.abs(random.nextInt()));
		// TODO: get rand int that is not already in pending or running blasts
		RpcBlasterCredentials blastOwnerInfo = new RpcBlasterCredentials(
				rpcBlasterInfo, Math.abs(random.nextInt()),
				UUID.randomUUID().toString(), Boolean.TRUE);
		pendingBlasts.put(blastOwnerInfo.getBlastId(), blastOwnerInfo);
		System.out.println("new blast put into pending blasts");
		return blastOwnerInfo;
	}

	public void startBlast(RpcBlasterCredentials blastOwnerInfo) {
		Blast existingBlast = findBlastByOwnerName(blastOwnerInfo.getRpcBlasterInfo().getName());
		if(existingBlast != null){
			return;
		}
		RpcBlasterCredentials found = pendingBlasts.get(blastOwnerInfo.getBlastId());
		if (found != null
				&& found.getRpcBlasterInfo().getId() ==
						blastOwnerInfo.getRpcBlasterInfo().getId()
				&& found.getKey().equals(blastOwnerInfo.getKey())
				&& found.isOwner()) {
			Blast blast = new Blast(found);
			liveBlasts.put(found.getBlastId(), blast);
			pendingBlasts.remove(blastOwnerInfo.getBlastId());
			// blast.start()
		}
	}
	
	public RpcBlasterCredentials createBlaster(String ownerName, String blasterName) 
	throws Exception{
		Blast blast = findBlastByOwnerName(ownerName);
		if(blast != null){
			Random random = new Random();
			int blasterId = Math.abs(random.nextInt());
			RpcBlasterInfo rpcBlasterInfo = new RpcBlasterInfo(blasterName, blasterId);
			RpcBlasterCredentials rpcBlasterCredentials = new RpcBlasterCredentials(
					rpcBlasterInfo, blast.getId(),
					UUID.randomUUID().toString(), Boolean.FALSE);
			this.pendingBlasters.put(blasterId, rpcBlasterCredentials);
			return rpcBlasterCredentials;
		}else{
			throw new Exception("blast with owner '" + ownerName +"' not found");
		}
	}
	
	public void joinBlast(RpcBlasterCredentials rpcBlasterCredentials) throws Exception{
		RpcBlasterCredentials foundCredentials = this.pendingBlasters.get(
				rpcBlasterCredentials.getRpcBlasterInfo().getId());
		if(foundCredentials != null
				&& foundCredentials.getBlastId() == rpcBlasterCredentials.getBlastId()
				&& foundCredentials.getKey().equals(rpcBlasterCredentials.getKey())){
			Blast blast = getBlast(foundCredentials.getBlastId());
			blast.addBlaster(foundCredentials);
		}else{
			throw new Exception("invalid or no pending blaster found matching " 
					+ rpcBlasterCredentials);
		}
	}
	
	private Blast findBlastByOwnerName(String ownerName) {
		Collection<Blast> blasts = this.liveBlasts.values();
		for(Blast blast : blasts){
			if(blast.getOwnerInfo().getName().equals(ownerName)){
				return blast;
			}
		}
		return null;
	}

	public Blast getBlast(int blastId) {
		return liveBlasts.get(blastId);
	}

	public void blastEnded(Blast blast) {
		liveBlasts.remove(blast.getId());
	}

	public void cleanup() {
		this.liveBlasts.clear();
		this.pendingBlasts.clear();
	}

	public static BlastRegistrar getBlastRegistrar(ServletContext servletContext) {
		return (BlastRegistrar) servletContext.getAttribute(NAME);
	}

	public static BlastRegistrar getBlastRegistrar(HttpServletRequest request) {
		return getBlastRegistrar(request.getSession().getServletContext());
	}

}
