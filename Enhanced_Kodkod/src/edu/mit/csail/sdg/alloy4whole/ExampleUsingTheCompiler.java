

/* Alloy Analyzer 4 -- Copyright (c) 2006-2009, Felix Chang
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify,
 * merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
 * OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

package edu.mit.csail.sdg.alloy4whole;

import static edu.mit.csail.sdg.ast.Sig.NONE;
import static edu.mit.csail.sdg.ast.Sig.SEQIDX;
import static edu.mit.csail.sdg.ast.Sig.SIGINT;
import static edu.mit.csail.sdg.ast.Sig.STRING;
import static edu.mit.csail.sdg.ast.Sig.UNIV;



import java.io.File;
import java.io.PrintWriter;
import java.time.Duration;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.Callable;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;
import java.util.concurrent.FutureTask;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.TimeoutException;

import edu.mit.csail.sdg.alloy4.A4Reporter;
import edu.mit.csail.sdg.alloy4.ConstList;
import edu.mit.csail.sdg.alloy4.Err;
import edu.mit.csail.sdg.alloy4.ErrorSyntax;
import edu.mit.csail.sdg.alloy4.ErrorWarning;
import edu.mit.csail.sdg.alloy4viz.VizGUI;
import edu.mit.csail.sdg.ast.Command;
import edu.mit.csail.sdg.ast.CommandScope;
import edu.mit.csail.sdg.ast.Module;
import edu.mit.csail.sdg.ast.Sig;
import edu.mit.csail.sdg.parser.CompUtil;
import edu.mit.csail.sdg.translator.A4Options;
import edu.mit.csail.sdg.translator.A4Solution;
import edu.mit.csail.sdg.translator.TranslateAlloyToKodkod;
//import edu.mit.csail.sdg.alloy4viz.VizGUI;


/** This class demonstrates how to access Alloy4 via the compiler methods. */

public final class ExampleUsingTheCompiler {
	 
    static class MyA4Reporter extends A4Reporter {
        int primaryVars = -1; 
        int totalVars = -1; 
        int clauses = -1;

        public void solve(int primaryVars, int totalVars, int clauses) {
            this.primaryVars = primaryVars;
            this.totalVars = totalVars;
            this.clauses = clauses;
            //System.out.println("primaryva");
        }

        public String toString() {
            //return "MyA4Reporter [primary vars: " + primaryVars + ";total vars: " + totalVars + "; clauses: " + clauses + "]";
        	return String.valueOf(primaryVars);
        }

    }
    
    static boolean check_pass(Sig s, int scope)
    {
    	if (s == UNIV)
           return false;
        if (s == SIGINT)
            return false;
        if (s == SEQIDX)
            return false;
        if (s == STRING) 
            return false;
        if (s == NONE)
            return false;
        if (s.isEnum != null)
            return false;
        if (s.isOne != null && scope != 1)
            return false;
        if (s.isLone != null && scope > 1)
            return false;
        if (s.isSome != null && scope < 1)
            return false;
        return true;
    }
    
    public static void main(String[] args) throws Exception {

        // Alloy4 sends diagnostic messages and progress reports to the A4Reporter.
        // By default, the A4Reporter ignores all these events (but you can extend the A4Reporter to display the event for the user)
        MyA4Reporter rep = new MyA4Reporter() {
            // For example, here we choose to display each "warning" by printing it to System.out
            @Override public void warning(ErrorWarning msg) {
                System.out.print("Relevance Warning:\n"+(msg.toString().trim())+"\n\n");
                System.out.flush();
            }
        };
       
        String sep = File.separator;
        String arg0= args[0];
        String specName = arg0.split(sep)[arg0.split(sep).length-1].replace(".als", "");
        System.out.println("Processing the Alloy specification: " + specName + ".als");
        System.out.println();
        Module world = CompUtil.parseEverything_fromFile(rep, null, arg0);           
        ConstList<Sig> sigs = world.getAllReachableSigs();
        // Choose some default options for how you want to execute the commands
        A4Options options = new A4Options();     
        options.symmetry = 20; // default setting
            	
        // prepare for the sat output directory
        File cnfFold = prepareOutputDir(specName, args[1]);
        // prepare for the sym output directory
        File symFold = prepareOutputDir(specName, args[2]);
            	
        int cmd_counter =0;
        for (Command command: world.getAllCommands()) {
            cmd_counter++;
            	   		
            File LabelFold = new File(cnfFold.getAbsolutePath() + sep + command.label.replace("$", ""));
            cnfGenerator(rep, sigs, command, options, LabelFold.getAbsolutePath(), cmd_counter);
                	
            LabelFold = new File(symFold.getAbsolutePath() + sep + command.label.replace("$", ""));
            permGenerator(rep, sigs, command, options, LabelFold.getAbsolutePath(), cmd_counter);

        }
    }

    
    static void cnfGenerator(A4Reporter rep, Iterable<Sig> sigs, Command cmd, A4Options opt, String LabelFold, int cmd_counter) throws Exception
    {
    	opt.solver = A4Options.SatSolver.CNF;
    	long startTime = System.currentTimeMillis();
        TranslateAlloyToKodkod.execute_command_simp(rep, sigs, cmd, opt, LabelFold, cmd_counter, 0);
        long endTime = System.currentTimeMillis();
        System.out.print("SAT formula generated!\n");
	System.out.print("SAT formula generation time: " );
	System.out.print(endTime-startTime);
	System.out.println(" milliseconds");
	System.out.println();
    }
    
    static void permGenerator(A4Reporter rep, Iterable<Sig> sigs, Command cmd, A4Options opt, String LabelFold, int cmd_counter) throws Exception
    {
    	opt.solver = A4Options.SatSolver.SBPERM;
    	long startTime = System.currentTimeMillis();
        TranslateAlloyToKodkod.execute_command_simp(rep, sigs, cmd, opt, LabelFold, cmd_counter, startTime);
	long endTime = System.currentTimeMillis();
	System.out.print("symmetry information extracted!\n");
	System.out.print("symmetry information extraction time: ");
	System.out.print(endTime-startTime);
	System.out.println(" milliseconds");
    }
    
    static File prepareOutputDir(String specName, String rootPath) {
    	String outputFoldPath = "";
	if(rootPath.endsWith(File.separator))
	    outputFoldPath = rootPath + specName;
	else
	    outputFoldPath = rootPath + File.separator + specName;
	    	
        File outputFold = new File(outputFoldPath);
        if(outputFold.exists())
            deleteDirectory(outputFold);
            
        if(!outputFold.exists())
            outputFold.mkdir();
            
        return outputFold;
    }
    
    static boolean deleteDirectory(File directoryToBeDeleted) {
        File[] allContents = directoryToBeDeleted.listFiles();
        if (allContents != null) {
            for (File file : allContents) {
                deleteDirectory(file);
            }
        }
        return directoryToBeDeleted.delete();
    }
}

