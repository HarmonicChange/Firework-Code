import java.awt.*;
import java.awt.event.*;

import javax.swing.*;

public class Display extends JFrame{
	// Define constants
	   public static final int CANVAS_WIDTH  = 450;
	   public static final int CANVAS_HEIGHT = 560;
	   
	   private static final int UPDATE_INTERVAL = 50; // milliseconds
	   
	   public int robot_x;
	   public int robot_y;
	   
	   Robot robot;
	   
	   Intersection[][] maze;
	 
	   // Declare an instance of the drawing canvas,
	   // which is an inner class called DrawCanvas extending javax.swing.JPanel.
	   private DrawCanvas canvas;
	 
	   // Constructor to set up the GUI components and event handlers
	   public Display(Intersection[][] my_maze, Robot r) {
		  maze = my_maze;
		  robot = r;
	      canvas = new DrawCanvas();    // Construct the drawing canvas
	      canvas.setPreferredSize(new Dimension(CANVAS_WIDTH, CANVAS_HEIGHT));
	 
	      // Set the Drawing JPanel as the JFrame's content-pane
	      Container cp = getContentPane();
	      cp.add(canvas);
	      // or "setContentPane(canvas);"
	 
	      setDefaultCloseOperation(EXIT_ON_CLOSE);   // Handle the CLOSE button
	      pack();              // Either pack() the components; or setSize()
	      setTitle("RobotSim");  // "super" JFrame sets the title
	      setVisible(true);    // "super" JFrame show
	      
	   // Create a new thread to run update at regular interval
	      Thread updateThread = new Thread() {
	         @Override
	         public void run() {
	            while (true) {
	               update(maze);   // update the (x, y) position
	               canvas.paintComponent(canvas.getGraphics());
	               //repaint();  // Refresh the JFrame. Called back paintComponent()
	               try {
	                  // Delay and give other thread a chance to run
	                  Thread.sleep(UPDATE_INTERVAL);  // milliseconds
	               } catch (InterruptedException ignore) {}
	            }
	         }
	      };
	      updateThread.start(); // called back run()
	   }
	   
	   public void update(Intersection[][] my_maze) {
		   maze = my_maze;
		   for(int i = 0; i < 5; i++) {
				for(int j = 0; j < 4; j++) {
					if(my_maze[i][j].robot) {
						   robot_x = 100*j + (j+1)*10;
						   robot_y = 100*i + (i+1)*10;
					   }
				}
			}
		   
	   }
	 
	   /**
	    * Define inner class DrawCanvas, which is a JPanel used for custom drawing.
	    */
	   private class DrawCanvas extends JPanel {
	      // Override paintComponent to perform your own painting
	      @Override
	      public void paintComponent(Graphics g) {
	         super.paintComponent(g);     // paint parent's background
	         for(int i = 0; i < 4; i++) {
	        	 for(int j = 0; j < 5; j++) {
	        		 g.setColor(Color.BLUE);
	        		 if(maze[j][i].wall_n) {g.fillRect(i*100 + i*10, j*100+j*10, 120, 10);}
	        		 if(maze[j][i].wall_w) {g.fillRect(i*100+i*10, j*100+j*10, 10, 120);}
	        		 if(maze[j][i].wall_s) {g.fillRect(i*100+i*10, (j+1)*100+(j+1)*10, 120, 10);}
	        		 if(maze[j][i].wall_e) {g.fillRect((i+1)*100+(i+1)*10, j*100+j*10, 10, 120);}
	        		 
	        		 g.setColor(Color.WHITE);
	        		 if(maze[j][i].explored)
	    	        	 g.fillRect((i)*100 + (i+1)*10, (j)*100+(j+1)*10, 100, 100);
	        	 }
	         }
	         setBackground(Color.BLACK);  // set background color for this JPanel
	 
	         // Your custom painting codes. For example,
	         // Drawing primitive shapes
	         //g.setColor(Color.YELLOW);    // set the drawing color
	         //g.drawLine(30, 40, 100, 200);
	         //g.drawOval(150, 180, 10, 10);
	         //g.drawRect(200, 210, 20, 30);
	         g.setColor(Color.RED);       // change the drawing color
	         //g.fillOval(300, 310, 30, 50);
	         
	         //g.fillRect(robot_x, robot_y, 100, 100);
	         g.fillRect(robot_x, robot_y, 100, 100);
	         
	         // Printing texts*/
	         g.setColor(Color.GREEN);
	         //g.setFont(new Font("Monospaced", Font.PLAIN, 12));
	         switch(robot.dir) {
	         case NORTH: g.drawString("O", robot_x+50, robot_y+10); break;
	         case WEST: g.drawString("O", robot_x+10, robot_y+50); break;
	         case SOUTH: g.drawString("O", robot_x+50, robot_y+100); break;
	         case EAST: g.drawString("O", robot_x+90, robot_y+50); break;
	         }
	         
	         /*g.fillRect(0, 0, CANVAS_WIDTH, 10);
	         g.fillRect(0, CANVAS_HEIGHT-10, CANVAS_WIDTH, 10);
	         g.fillRect(0, 0, 10, CANVAS_HEIGHT);
	         g.fillRect(CANVAS_WIDTH-10, 0, 10, CANVAS_HEIGHT);*/
	         
	         
	      }
	   }
}
