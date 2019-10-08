class BootStrap {
	
	def authenticateService
	
     def init = { servletContext ->
     	if(!Role.count()){
			new Role(description: "admin", authority: "ROLE_ADMIN").save()
			Role role = new Role(description: "user", authority: "ROLE_USER").save()
			Blaster blaster = new Blaster(username: "john", userRealName: "John McCaughey", email: "webhuddle@gmail.com",
				enabled: true, passwd: authenticateService.encodePassword("password"));
			blaster.save();
			role.addToPeople(blaster)
			blaster.save()
		}
     }
     def destroy = {
     }
} 