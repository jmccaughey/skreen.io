package com.deskblast.server;

import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;

import com.deskblast.client.RpcBlasterCredentials;
import com.deskblast.client.RpcBlasterInfo;
import com.deskblast.server.json.BlasterCredentials;

public class StartBlastServlet extends AbstractJsonServlet{

	@Override
	public Object handleGetRequest(HttpServletRequest httpServletRequest)
			throws ServletException {
		throw new ServletException("start blast via POST request");
	}

	@Override
	public Object handlePostRequest(HttpServletRequest httpServletRequest,
			Object object) throws ServletException {
		//throw new RuntimeException("this should not be used anymore");

		BlasterCredentials bc = (BlasterCredentials)object;
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
		
		
//		BlastRegistrar.getBlastRegistrar(httpServletRequest)
//			.startBlast((BlasterCredentials)object);
		return null;
	} 

}
