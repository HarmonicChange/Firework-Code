
public class Node {
	public Node n;
	public Node e;
	public Node s;
	public Node w;
	
	public Node parent;
	
	public int x;
	public int y;
	
	public boolean explored;
	
	public Node(int my_x, int my_y, boolean exp, Node nn, Node en, Node sn, Node wn, Node p) {
		x = my_x;
		y = my_y;
		explored = exp;
		n = nn;
		e = en;
		s = sn;
		w = wn;
		parent = p;
	}
}
