
public class Intersection {
	public boolean wall_n;
	public boolean wall_e;
	public boolean wall_s;
	public boolean wall_w;
	
	public boolean treasure;
	
	public boolean explored;
	
	public boolean robot;
	
	public Intersection(boolean my_wall_n, boolean my_wall_e,
			boolean my_wall_s, boolean my_wall_w, boolean my_treasure, boolean my_explored, boolean my_robot) {
		wall_n = my_wall_n;
		wall_e = my_wall_e;
		wall_s = my_wall_s;
		wall_w = my_wall_w;
		
		treasure = my_treasure;
		explored = my_explored;
		
		robot = my_robot;
	}
	
}
