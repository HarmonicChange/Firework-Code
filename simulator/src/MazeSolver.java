/*MazeSolver creates a random maze as a collection of intersections.
The maze can be updated using the updateMaze method to reflect the current location
of the robot. This method also updates the robot's sensors.*/

import javax.swing.SwingUtilities;
import java.util.concurrent.TimeUnit;

public class MazeSolver {
	
	public static Intersection[][] maze = new Intersection[5][4];
	
	public MazeSolver(Robot r) {
		boolean robot;
		boolean wall_n;
		boolean wall_e;
		boolean wall_s;
		boolean wall_w;
		boolean explored;
		for(int i = 0; i < 5; i++) {
			for(int j = 0; j < 4; j++) {
				
				wall_n = (Math.random() < 0.20 );
				wall_e = (Math.random() < 0.20 );
				wall_s = (Math.random() < 0.20 );
				wall_w = (Math.random() < 0.20 );
				
				if(i==0)wall_n = true;
				if(i==4)wall_s = true;
				if(j==0)wall_w = true;
				if(j==3)wall_e = true;
				
				if (i == r.y && j == r.x) {
					explored = true;
					robot = true;
				}
				else {
					explored = false;
					robot = false;
				}
				
				maze[i][j] = new Intersection(wall_n, wall_e, wall_s, wall_w, false, explored, robot);
			}
		}
	}
	
	public void updateMaze(Robot r) {
		for(int i = 0; i < 5; i++) {
			for(int j = 0; j < 4; j++) {
				if (i == r.y && j == r.x) {
					maze[i][j].explored = true;
					maze[i][j].robot = true;
					
						switch(r.dir) {
						case NORTH: {
							if(i == 0) r.sensor_f = (maze[i][j].wall_n);
							else r.sensor_f = (maze[i][j].wall_n || maze[i-1][j].wall_s);
							
							if(j == 0)r.sensor_r = maze[i][j].wall_e;
							else r.sensor_r = (maze[i][j].wall_e || maze[i][j+1].wall_w);
							
							if(j == 3)r.sensor_l = maze[i][j].wall_w;
							else r.sensor_l = (maze[i][j].wall_w || maze[i][j-1].wall_e);
							
							break;
						}
						case EAST: {
							if(j==3)r.sensor_f = maze[i][j].wall_e;
							else r.sensor_f = (maze[i][j].wall_e || maze[i][j+1].wall_w);
							
							if(i == 4)r.sensor_r = maze[i][j].wall_s;
							else r.sensor_r = (maze[i][j].wall_s || maze[i+1][j].wall_n);
							
							if(i == 0)r.sensor_l = maze[i][j].wall_n;
							else r.sensor_l = (maze[i][j].wall_n || maze[i-1][j].wall_s);
							
							break;
						}
						case SOUTH: {
							if(i==4)r.sensor_f = maze[i][j].wall_s;
							else r.sensor_f = (maze[i][j].wall_s || maze[i+1][j].wall_n);
							
							if(j==0) r.sensor_r = maze[i][j].wall_w ;
							else r.sensor_r = (maze[i][j].wall_w || maze[i][j-1].wall_e);
							
							if(j==3)r.sensor_l = maze[i][j].wall_e;
							else r.sensor_l = (maze[i][j].wall_e || maze[i][j+1].wall_w);
								
							break;
						}
						case WEST: {
							if(j==0) r.sensor_f = maze[i][j].wall_w;
							else r.sensor_f = (maze[i][j].wall_w || maze[i][j-1].wall_e);
							
							if(i==0)r.sensor_r = maze[i][j].wall_n;
							else r.sensor_r = (maze[i][j].wall_n || maze[i-1][j].wall_s);
							
							if(i==4)r.sensor_l = maze[i][j].wall_s;
							else r.sensor_l = (maze[i][j].wall_s || maze[i+1][j].wall_n);
							
							break;
						}
						}
				}
				else maze[i][j].robot = false;				
			}
		}
	}
	
	// The entry main method
	   public static void main(String[] args) {
		  Robot r = new Robot(3,4, Direction.NORTH);
		  MazeSolver ms = new MazeSolver(r);
	      // Run the GUI codes on the Event-Dispatching thread for thread safety
	      SwingUtilities.invokeLater(new Runnable() {
	         @Override
	         public void run() {
	            new Display(maze, r); // Let the constructor do the job
	         }
	      });
	      
	      while(!r.done) {
	    	  ms.updateMaze(r);
	    	  
		      try {
				TimeUnit.MILLISECONDS.sleep(500);
		      } catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
		      }
		      r.navigate();
	      }
	      System.out.println("Done!");
	   }
}
