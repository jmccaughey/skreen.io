package com.deskblast.server;

import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;

import com.deskblast.client.RpcBlasterCredentials;
import com.deskblast.client.RpcBlasterInfo;
import com.deskblast.server.json.BlasterCredentials;
import com.deskblast.server.json.BlasterInfo;

public class CreateAndStartBlastServlet extends AbstractJsonServlet{

	@Override
	public Object handleGetRequest(HttpServletRequest httpServletRequest) throws ServletException {
		BlastRegistrar blastRegistrar = 
				BlastRegistrar.getBlastRegistrar(httpServletRequest);
		String ownerName = httpServletRequest.getParameter("ownerName"); 
		RpcBlasterCredentials blasterCredentials = null;
		if(ownerName != null){
			blasterCredentials = blastRegistrar.createBlast(ownerName);
		}else{
			blasterCredentials = blastRegistrar.createBlast();
		}
		BlasterInfo bi = new BlasterInfo(
				blasterCredentials.getRpcBlasterInfo().getName(), 
				blasterCredentials.getRpcBlasterInfo().getId());
		BlasterCredentials bc = new BlasterCredentials(
				bi, 
				blasterCredentials.getBlastId(), 
				blasterCredentials.getKey(),
				blasterCredentials.isOwner()); 
		//BlasterCredentials bc = (BlasterCredentials)object;
		RpcBlasterCredentials rbc = new RpcBlasterCredentials(
				new RpcBlasterInfo(bc.getBlasterInfo().getName(), bc.getBlasterInfo().getId()), 
				bc.getBlastId(), bc.getKey(), bc.getOwner());
		try {
			BlastRegistrar.getBlastRegistrar(httpServletRequest)
				.startBlast(rbc);
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			throw new ServletException("problem starting blast", e);
		}
		return bc;
	}

	@Override
	public Object handlePostRequest(HttpServletRequest httpServletRequest,
			Object object) throws ServletException {
		return handleGetRequest(httpServletRequest);
	} 

}
