import java.util.*;

public class Sort{
	public static void main(String[] args){
		Scanner scanner = new Scanner(System.in);
		int input = 0;
		boolean flag = true;
		do{
			System.out.println("please enter your choice");
			System.out.println("> 1: ");
			System.out.println("> 2: ");
			System.out.println("> 3: ");
			System.out.println("> 3: ");
			//System.out.println("> 5: ");
			//System.out.println("> 6: ");
			//System.out.println("> 7: ");
			//System.out.println("> 0: EXIT");
			input = scanner.nextInt();

			if (input == 0)
				flag = false;
		}while (flag);
	}
	
}
