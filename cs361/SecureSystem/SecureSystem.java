import java.util.Map;
import java.util.TreeMap;

public class SecureSystem {
	private ObjectElem o1;
	private ObjectElem o2;
	private SubjectElem s1;
	private SubjectElem s2;
	private InstructionObject ino;
	public SecureSystem(ObjectElem o1, ObjectElem o2, SubjectElem s1, SubjectElem s2, InstructionObject ino){
	 this.o1 = o1;
	 this.o2 = o2;
	 this.s1 = s1;
	 this.s2 = s2;
	 this.ino = ino;
	}
	public ObjectElem getO1() {
		return o1;
	}

	public void setO1(ObjectElem o1) {
		this.o1 = o1;
	}

	public ObjectElem getO2() {
		return o2;
	}

	public void setO2(ObjectElem o2) {
		this.o2 = o2;
	}

	public SubjectElem getS1() {
		return s1;
	}

	public void setS1(SubjectElem s1) {
		this.s1 = s1;
	}

	public SubjectElem getS2() {
		return s2;
	}

	public void setS2(SubjectElem s2) {
		this.s2 = s2;
	}

	public InstructionObject getIno() {
		return ino;
	}

	public void setIno(InstructionObject ino) {
		this.ino = ino;
	}

	private void printState(){
		switch(ino.getOperation()){
			case READ:
				System.out.println(ino.getSubject().toLowerCase() + " reads " + ino.getObject().toLowerCase());
				break;
			case WRITE:
				System.out.println(ino.getSubject().toLowerCase() + " writes value " + ino.getValue() + " to " + ino.getObject().toLowerCase());
				break;
			case BAD:
				System.out.println("Bad Instruction");
				break;
			default:
				break;
		}

		System.out.println("The current state is:");
		System.out.println("	LObj has value: " + o2.getValue());
		System.out.println("	HObj has value: " + o1.getValue());
		System.out.println("	Lyle has recently read: " + s2.getTemp());
		System.out.println("	Hal has recently read: " + s1.getTemp());
		System.out.println();
	}
	
	public static void main(String [] args){
		InstructionObject ino = new InstructionObject(args[0]);
		ObjectElem HObj = new ObjectElem("HObj");
		ObjectElem LObj = new ObjectElem("LObj");
		SubjectElem Hal = new SubjectElem("Hal");
		SubjectElem Lyle = new SubjectElem("Lyle");
		SecureSystem sys = new SecureSystem(HObj, LObj, Hal, Lyle, ino);
		Map<String, SecurityLevel> oMap = new TreeMap<String, SecurityLevel>(String.CASE_INSENSITIVE_ORDER);
		Map<String, SecurityLevel> sMap = new TreeMap<String, SecurityLevel>(String.CASE_INSENSITIVE_ORDER);
		SecurityLevel low = SecurityLevel.LOW;
		SecurityLevel high = SecurityLevel.HIGH;
		sMap.put("Hal", high);
		sMap.put("Lyle", low);
		oMap.put("HObj", high);
		oMap.put("LObj", low);
		ReferenceMonitor monitor = new ReferenceMonitor(ino, oMap, sMap);
		monitor.addObject(HObj);
		monitor.addObject(LObj);
		monitor.addSubject(Hal);
		monitor.addSubject(Lyle);
		
		while(ino.getScanner().hasNextLine()){
			ino.parseNextLine();
			boolean valid = false;
			for(String s: sMap.keySet()){
				if(s.equalsIgnoreCase(ino.getSubject())){
					valid = true;
					break;
				}
			}
			for(String s: oMap.keySet()){
				if(s.equalsIgnoreCase(ino.getObject())){
					valid = true;
					break;
				}
			}
			if(!valid){
				ino.setOperation(Operation.BAD);
			}
			boolean hasPermission = monitor.permission();
			if(ino.getOperation() == Operation.READ){
				if(sys.getS1().getName().equalsIgnoreCase(ino.getSubject())){
					sys.setS1(monitor.getSubject());
				}
				else if(sys.getS2().getName().equalsIgnoreCase(ino.getSubject())){
					sys.setS2(monitor.getSubject());
				}
			}
			if(ino.getOperation() == Operation.WRITE && hasPermission){
				if(sys.getO1().getName().equalsIgnoreCase(ino.getObject())){
					sys.setO1(monitor.getObject());
				}
				else if(sys.getO2().getName().equalsIgnoreCase(ino.getObject())){
					sys.setO2(monitor.getObject());
				}
			}
			sys.printState();
		}
	}
}
