import org.apache.commons.httpclient.HttpClient;
import org.apache.commons.httpclient.HttpMethod;
import org.apache.commons.httpclient.methods.GetMethod;

class RoomController {

	def authenticateService

	// the delete, save and update actions only accept POST requests
//	static Map allowedMethods = [delete: 'POST', save: 'POST', update: 'POST']

	def index = {
		
		// if user is logged in and name == params.blaster,
		// consider it a request to start a meeting
		// talk to createMeeting servlet and pass
		// credentials in model to deskblast view
		
		// if user is not logged in, check if params.blaster
		// is a valid user.  If so, pass blaster name
		// in model to deskblast view
		
		def blaster = Blaster.findByUsername(params.blaster)
		if(blaster){
			String schemeHostPort = "http://localhost:" + request.getLocalPort() 
			if(authenticateService.isLoggedIn() 
					&& blaster.username == authenticateService.principal().username){
				log.error blaster.email + " start meeting request "
				// TODO: create meeting right here, talking to servlet
				// servlet trusts create meeting requests from 
				// non-routable IPs
				HttpClient client = new HttpClient()
				HttpMethod method = new GetMethod(schemeHostPort + "/deskblast/createBlast?ownerName=" + blaster.username)
				client.executeMethod(method)
				String response = method.getResponseBodyAsString();
				def map = [hostCredentials: response]
				render(view:"/deskblast/deskblast", model:map)
			}else{
				// eventually this will require OK from host, so should
				// be async.  For now accept all entry requests
				log.error("join meeting request [" + blaster.username + "]")
				HttpClient client = new HttpClient()
				HttpMethod method = new GetMethod(schemeHostPort + "/deskblast/createBlaster?ownerName=" + blaster.username + "&joinerName=guest_" + new Date().time)
				client.executeMethod(method)
				String response = method.getResponseBodyAsString();	
				def map = [guestCredentials: response]
				render(view:"/deskblast/deskblast", model:map)			
			}
		}else{
			flash.message = "User '" + params.blaster + "' not found."
			render(view:"/index")
		}
		return
	}
}
