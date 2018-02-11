//import java.*

public class test {
    public static void main(String[] args) {
        int a[] = {10, 20, 30, 40, 50, 60, 70}, i, sum=0;
        for(i=0;i<a.length;i++) {
            if(i%3==1) {
                sum = sum + a[i];
            }
        }
        System.out.println(sum);
    }
}
