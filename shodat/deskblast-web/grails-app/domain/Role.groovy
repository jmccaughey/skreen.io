



/**
 * Authority domain class.
 */
class Role {

	static hasMany = [people: Blaster]

	/** description */
	String description
	/** ROLE String */
	String authority

	static constraints = {
		authority(blank: false, unique: true)
		description()
	}
}
