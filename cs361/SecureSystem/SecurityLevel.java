
public enum SecurityLevel {
	LOW,
	HIGH;
	public boolean dominates(SecurityLevel other){
		if(this==LOW && other==HIGH){
			return false;
		}
		else{
			return true;
		}
	}
}
