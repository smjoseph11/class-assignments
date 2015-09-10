import java.util.ArrayList;
import java.util.Map;
public class ReferenceMonitor{
	private ArrayList<ObjectElem> objList;
	private ArrayList<SubjectElem> subjList;
	private ObjectElem object;
	private SubjectElem subject;
	private InstructionObject instr;
	private Map<String, SecurityLevel> oMap;
	private Map<String, SecurityLevel>sMap;

	ReferenceMonitor(InstructionObject instr, Map<String, SecurityLevel> oMap, Map<String, SecurityLevel> sMap){
		this.objList = new ArrayList<ObjectElem>();
		this.subjList = new ArrayList<SubjectElem>();
		this.object = new ObjectElem();
		this.subject = new SubjectElem();
		this.instr = instr;
		this.oMap = oMap;
		this.sMap = sMap;
	}
	ReferenceMonitor(ArrayList<ObjectElem> objList, ArrayList<SubjectElem> subjList, InstructionObject instr){
		this.objList = objList;
		this.subjList = subjList; 
		this.instr = instr; 
	}
	public InstructionObject getInstructionObject(){
		return instr; 
	}
	public void setInstructionObject(InstructionObject instr){
		this.instr = instr;
	}
	public ArrayList<ObjectElem> getObjList() {
		return objList;
	}
	public void setObjList(ArrayList<ObjectElem> objList) {
		this.objList = objList;
	}
	public ArrayList<SubjectElem> getSubjList() {
		return subjList;
	}
	public void setSubjList(ArrayList<SubjectElem> subjList) {
		this.subjList = subjList;
	}
	
	public ObjectElem getObject() {
		return object;
	}
	public SubjectElem getSubject() {
		return subject;
	}
	public void addObject(ObjectElem o){
		objList.add(o);
	}
	public void addSubject(SubjectElem s){
		subjList.add(s);
	}
	public boolean permission(){
		ObjectManager manager = new ObjectManager();
		ArrayList<String> instructions = instr.getParsedLine();
		switch(instr.getOperation()){
			case READ:
				if(sMap.get(instructions.get(1)).dominates(oMap.get(instructions.get(2)))){
					manager.processInstruction();
					return true;
				}
				else{
					SubjectElem newElem = new SubjectElem(instructions.get(1), 0);
					subject = newElem;
				}
				break;
			case WRITE:
				if(oMap.get(instructions.get(2)).dominates(sMap.get(instructions.get(1)))){
					manager.processInstruction();
					return true;
				}
				break;
			case BAD:
				return false;
			default:
				return false;
		}
		return false;
	}
	public String toString(){
		String s = "{";
		for(ObjectElem o: objList){
			s+=o.toString() + ", ";
		}
		s+="}, {";
		for(SubjectElem se: subjList){
			s+=se.toString()+", ";
		}
		s+="}";
		return s;
	}
	class ObjectManager {
		public ObjectManager(){
			
		}
		public void processInstruction(){
			ArrayList<String> instructions = instr.getParsedLine();
			switch(instr.getOperation()){
				case READ:
					for(ObjectElem o: objList){
						if(o.getName().equalsIgnoreCase(instructions.get(2))){
							for(int i=0; i<subjList.size(); i++){
								if(subjList.get(i).getName().equalsIgnoreCase(instructions.get(1))){
									SubjectElem newElem = new SubjectElem(instructions.get(1), o.getValue());
									subjList.set(i, newElem);
									subject = subjList.get(i);
									break;
								}
							}
							break;
						}
					}	
					break;
				case WRITE:
					for(SubjectElem s: subjList){
						if(s.getName().equalsIgnoreCase(instructions.get(1))){
							for(int i=0; i<objList.size(); i++){
								if(objList.get(i).getName().equalsIgnoreCase(instructions.get(2))){
									ObjectElem newElem = new ObjectElem(instructions.get(2), 
												Integer.parseInt(instructions.get(3)));
									objList.set(i, newElem);
									object = objList.get(i);
									break;
								}
							}
							break;
						}
					}
					break;
				case BAD:
					break;
				default:
					break;
			}
		}
	}
}