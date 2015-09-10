import java.io.File;
import java.io.FileNotFoundException;
import java.util.ArrayList;
import java.util.Scanner;
import java.util.StringTokenizer;
public class InstructionObject {
	private ArrayList<String> parsedLine;
	private File file;
	private Scanner sc;
	private Operation operation;
	private String subject;
	private String object; 
	private int value; 
	public InstructionObject(String filename){
		parsedLine = new ArrayList<String>();
		file = new File(filename);
		try {
		    sc = new Scanner(file);
		} 
		catch (FileNotFoundException e) {
			e.printStackTrace();
		}
	}
	public ArrayList<String> getParsedLine(){
		return parsedLine;
	}
	public Operation getOperation(){
		return operation;
	}
	public void setOperation(Operation operation) {
		this.operation = operation;
	}
	public String getSubject() {
		return subject;
	}
	public String getObject() {
		return object;
	}
	public int getValue(){
		return value;
	}
	public Scanner getScanner(){
		return sc;
	}
	public void parseNextLine(){
		if(parsedLine.size() > 0)
			parsedLine.clear();
		if(sc.hasNextLine()){
			String line = sc.nextLine();
			StringTokenizer tokenizer = new StringTokenizer(line);
			while(tokenizer.hasMoreTokens()){
				parsedLine.add(tokenizer.nextToken());
			}
			if(parsedLine.get(0).equalsIgnoreCase("read") && parsedLine.size() == 3){
				this.operation = Operation.READ;
				this.subject = parsedLine.get(1);
				this.object = parsedLine.get(2);
			}
			else if(parsedLine.get(0).equalsIgnoreCase("write") && parsedLine.size() == 4){
				this.operation = Operation.WRITE;
				this.subject = parsedLine.get(1);
				this.object = parsedLine.get(2);
				this.value = Integer.parseInt(parsedLine.get(3));
			}
			else{
				this.operation = Operation.BAD;
			}
		}
		else{
			this.operation = Operation.EOF;
		}
	}
}
