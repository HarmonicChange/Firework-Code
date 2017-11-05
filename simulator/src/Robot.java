/*Robot represents our robot in the maze. Programming the robot to
navigate the maze will only require editing this file. The sensor and location readings
are updated as the robot explores*/

import java.util.ArrayList;

public class Robot {
	
	public int x;
	public int y;
	public Direction dir;
	
	public boolean sensor_f;
	public boolean sensor_r;
	public boolean sensor_l;
	public boolean treasure_sensor;
	
	public Node curr_node;
	public boolean[][] seen;
	
	public Node[][] maze = new Node[5][4];;
	
	public int unexplored;
	public boolean done;
	
	public Node home;
	
	public Robot(int my_x, int my_y, Direction my_dir) {
		done = false;
		unexplored = 1;
		x = my_x;
		y = my_y;
		dir = my_dir;
		curr_node = new Node(x,y,false,null,null,null,null,null); 
		home = curr_node;
		seen = new boolean[5][4];
	}
	
	/*navigate() is called on loop. The objective of navigate is for the robot to make
	one action. an action can either be a right turn, left turn, or move forward*/
	public void navigate() {
		//if(unexplored == 0)done = true;
		update();
		Node rn, fn, ln;
		switch(dir) {
		case NORTH: {
			rn = curr_node.e;
			fn = curr_node.n;
			ln = curr_node.w;
			break;
		}
		case EAST: {
			rn = curr_node.s;
			fn = curr_node.e;
			ln = curr_node.n;
			break;
		}
		case SOUTH: {
			rn = curr_node.w;
			fn = curr_node.s;
			ln = curr_node.e;
			break;
		}
		case WEST: {
			rn = curr_node.n;
			fn = curr_node.w;
			ln = curr_node.s;
			break;
		}
		default: {
			rn = curr_node.e;
			fn = curr_node.n;
			ln = curr_node.w;
			break;
		}
		}
		//Random
		/*if(rn != null && !sensor_r && !rn.explored)turnRight();
		else if(fn != null && !sensor_f && !fn.explored)moveForward();
		else if(ln != null && !sensor_l && !ln.explored)turnLeft();
		else if(rn != null && ln != null && fn != null && !sensor_r && !sensor_l && !sensor_f) {
			if(Math.random()>0.8)turnRight();
			else if(Math.random()>0.8)turnLeft();
			else moveForward();
		}
		else if(rn != null && ln != null && !sensor_r && !sensor_l) {
			if(Math.random()>0.5)turnRight();
			else turnLeft();
		}
		else if(fn != null && rn != null && !sensor_r && !sensor_f) {
			if(Math.random()>0.8)turnRight();
			else moveForward();
		}
		else if(fn != null && ln != null && !sensor_f && !sensor_l) {
			if(Math.random()>0.2)moveForward();
			else turnLeft();
		}
		else if(fn != null && !sensor_f)moveForward();
		else if(ln != null && !sensor_l)turnLeft();
		else if(rn != null && !sensor_r)turnRight();
		else turnRight();*/
		
		//Dfs
		/*
		if(rn != null && !sensor_r && !rn.explored)turnRight();
		else if(fn != null && !sensor_f && !fn.explored)moveForward();
		else if(ln != null && !sensor_l && !ln.explored)turnLeft();
		else if((rn == null || rn.explored) && (ln == null || ln.explored) && (fn == null || fn.explored)) {
			
			if(curr_node == home)done = true;
			
			if(rn == curr_node.parent)
				turnRight();
			else if(ln == curr_node.parent)
				turnLeft();
			else if(fn == curr_node.parent)
				moveForward();
			else turnRight();
		}*/
	}
	
	public void update() {
		maze[y][x] = curr_node;
		seen[y][x] = true;
		
		switch(dir) {
		case NORTH:{
			if(curr_node.y!=0 && curr_node.n == null && !sensor_f && !seen[y-1][x]) {
				curr_node.n = new Node(x, y-1, false, null, null, curr_node, null, curr_node);
				seen[y-1][x] = true;
				unexplored++;
			}
			if(curr_node.x!=3 && curr_node.e == null && !sensor_r && !seen[y][x+1]) {
				curr_node.e = new Node(x+1, y, false, null, null, null, curr_node, curr_node);
				seen[y][x+1] = true;
				unexplored++;
			}
			if(curr_node.x!=0 && curr_node.w == null && !sensor_l && !seen[y][x-1]) {
				curr_node.w = new Node(x-1, y, false, null, curr_node, null, null, curr_node);
				seen[y][x-1] = true;
				unexplored++;
			}
			break;
		}
		case EAST: {
			if(curr_node.x!=3 && curr_node.e == null && !sensor_f && !seen[y][x+1]) {
				curr_node.e = new Node(x+1, y, false, null, null, null, curr_node, curr_node);
				seen[y][x+1] = true;
				unexplored++;
			}
			if(curr_node.y!=4 && curr_node.s == null && !sensor_r && !seen[y+1][x]) {
				curr_node.s = new Node(x, y+1, false, curr_node, null, null, null, curr_node);
				seen[y+1][x] = true;
				unexplored++;
			}
			if(curr_node.y!=0 && curr_node.n == null && !sensor_l && !seen[y-1][x]) {
				curr_node.n = new Node(x, y-1, false, null, null, curr_node, null, curr_node);
				seen[y-1][x] = true;
				unexplored++;
			}
			break;
		}
		case SOUTH: {
			if(curr_node.y!=4 && curr_node.s == null && !sensor_f && !seen[y+1][x]) {
				curr_node.s = new Node(x, y+1, false, curr_node, null, null, null, curr_node);
				seen[y+1][x] = true;
				unexplored++;
			}
			if(curr_node.x!=0 && curr_node.w == null && !sensor_r && !seen[y][x-1]) {
				curr_node.w = new Node(x-1, y, false, null, curr_node, null, null, curr_node);
				seen[y][x-1] = true;
				unexplored++;
			}
			if(curr_node.x!=3 && curr_node.e == null && !sensor_l && !seen[y][x+1]) {
				curr_node.e = new Node(x+1, y, false, null, null, null, curr_node, curr_node);
				seen[y][x+1] = true;
				unexplored++;
			}
			break;
		}
		case WEST: {
			if(curr_node.x!=0 && curr_node.w == null && !sensor_f && !seen[y][x-1]) {
				curr_node.w = new Node(x-1, y, false, null, curr_node, null, null, curr_node);
				seen[y][x-1] = true;
				unexplored++;
			}
			if(curr_node.y!=0 && curr_node.n == null && !sensor_r && !seen[y-1][x]) {
				curr_node.n = new Node(x, y-1, false, null, null, curr_node, null, curr_node);
				seen[y-1][x] = true;
				unexplored++;
			}
			if(curr_node.y!=4 && curr_node.s == null && !sensor_l && !seen[y+1][x]) {
				curr_node.s = new Node(x, y+1, false, curr_node, null, null, null, curr_node);
				seen[y+1][x] = true;
				unexplored++;
			}
			break;
		}
		}
		if(!curr_node.explored) {
			curr_node.explored = true;
			unexplored--;
		}
	}
	
	public void moveForward() {
		switch(dir){
		case NORTH: {y--; curr_node = curr_node.n; break;}
		case EAST: {x++; curr_node = curr_node.e; break;}
		case SOUTH: {y++; curr_node = curr_node.s; break;}
		case WEST:{ x--; curr_node = curr_node.w; break;}
		default:
			break;
		}
	}
	public void turnLeft() {
		switch(dir){
		case NORTH: {dir = Direction.WEST; break;}
		case EAST: {dir = Direction.NORTH; break;}
		case SOUTH: {dir = Direction.EAST; break;}
		case WEST: {dir = Direction.SOUTH; break;}
		default:
			break;
		}	
	}
	public void turnRight() {
		switch(dir){
		case NORTH: {dir = Direction.EAST; break;}
		case EAST: {dir = Direction.SOUTH; break;}
		case SOUTH: {dir = Direction.WEST; break;}
		case WEST: {dir = Direction.NORTH; break;}
		default:
			break;
		}
	}
}
