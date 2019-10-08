package com.deskblast.server;

import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;

import com.deskblast.client.RpcBlasterCredentials;
import com.deskblast.client.RpcBlasterInfo;
import com.deskblast.server.json.BlasterCredentials;

public class JoinBlastServlet extends AbstractJsonServlet{

	@Override
	public Object handleGetRequest(HttpServletRequest httpServletRequest)
			throws ServletException {
		throw new ServletException("start blast via POST request");
	}

	@Override
	public Object handlePostRequest(HttpServletRequest httpServletRequest,
			Object object) throws ServletException {
		//throw new RuntimeException("this should not be used anymore");
		try {
			BlasterCredentials bc = (BlasterCredentials)object;
			RpcBlasterCredentials rbc = new RpcBlasterCredentials(
					new RpcBlasterInfo(bc.getBlasterInfo().getName(), bc.getBlasterInfo().getId()), 
					bc.getBlastId(), bc.getKey(), bc.getOwner());
			BlastRegistrar.getBlastRegistrar(httpServletRequest)
				.joinBlast(rbc);
		} catch (Exception e) {
			e.printStackTrace();
			throw new ServletException("problem joining", e);
		}
		return null;
	} 

}
