
public class ObjectElem {
	private String name;
	private int value;
	
	public ObjectElem(){
		this.name = "";
		this.value = 0;
	}
	public ObjectElem(String name){
		this.name = name;
		this.value = 0;
	}
	public ObjectElem(String name, int value){
		this.name = name;
		this.value = value;
	}
	public String getName() {
		return name;
	}
	public void setName(String name) {
		this.name = name;
	}
	public int getValue() {
		return value;
	}
	public void setValue(int value) {
		this.value = value;
	}
	public String toString(){
		return "(Object Name: " + name + ", Object Value: "+ value + ")";
	}

}
