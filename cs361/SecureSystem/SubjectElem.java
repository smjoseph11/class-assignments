
public class SubjectElem {
	private String name;
	private int temp = 0;
	public SubjectElem(){
		this.name = "";
		this.temp = 0;
	}
	public SubjectElem(String name){
		this.name = name;
		this.temp = 0;
	}
	public SubjectElem(String name, int temp){
		this.name = name;
		this.temp = temp;
	}
	public String getName() {
		return name;
	}
	public void setName(String name) {
		this.name = name;
	}
	public int getTemp() {
		return temp;
	}
	public void setTemp(int temp) {
		this.temp = temp;
	} 
	public String toString(){
		return "(Subject Name: " + name + ", Subjet Temp: "+ temp + ")";
	}
}
