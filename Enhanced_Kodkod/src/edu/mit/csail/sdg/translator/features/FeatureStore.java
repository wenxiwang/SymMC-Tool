package edu.mit.csail.sdg.translator.features;

import java.util.Arrays;
import java.util.List;

public class FeatureStore {

  public String modelName;
  public String cmdName;

  public int relationNum;
  public int intConstNum;
  public int constFormulaNum;
  public int varNum;
  public int constExprNum;
  public int notFormulaNum;
  public int intToExprCastNum;
  public int declNum;
  public int exprToIntCastNum;
  public int unaryExprNum;
  public int unaryIntExprNum;
  public int multiFormulaNum;
  public int binaryExprNum;
  public int compFormulaNum;
  public int binaryFormulaNum;
  public int binaryIntExprNum;
  public int intCompFormula;
  public int ifExprNum;
  public int ifIntExprNum;
  public int sumExprNum;
  public int quantFormulaNum;
  public int fixFormulaNum;
  public int comprehensionNum;
  public int declsNum;
  public int projectExprNum;
  public int relationPredNum;
  public int naryExprNum;
  public int naryIntExprNum;
  public int naryFormula;  
  public int totalNodeNum;
  public int nodeHeight;
  public int nodeDiameter;
  public int closureNum;
  public int relationinboundsNum;


  public FeatureStore(String modelName, String cmdName) {
    this.modelName = modelName;
    this.cmdName = cmdName;
    this.relationNum = 0;
    this.intConstNum = 0;
    this.constFormulaNum = 0;
    this.varNum = 0;
    this.constExprNum = 0;
    this.notFormulaNum = 0;
    this.intToExprCastNum = 0;
    this.declNum = 0;
    this.exprToIntCastNum = 0;
    this.unaryExprNum = 0;
    this.unaryIntExprNum = 0;
    this.multiFormulaNum = 0;
    this.binaryExprNum = 0;
    this.compFormulaNum = 0;
    this.binaryFormulaNum = 0;
    this.binaryIntExprNum = 0;
    this.intCompFormula = 0;
    this.ifExprNum = 0;
    this.ifIntExprNum = 0;
    this.sumExprNum = 0;
    this.quantFormulaNum = 0;
    this.fixFormulaNum = 0;
    this.comprehensionNum = 0;
    this.declsNum = 0;
    this.projectExprNum = 0;
    this.relationPredNum = 0;
    this.naryExprNum = 0;
    this.naryIntExprNum = 0;
    this.naryFormula = 0;
    this.totalNodeNum = 0;
    this.nodeHeight = 0;
    this.nodeDiameter = 0;
    this.closureNum = 0;
    this.relationinboundsNum =0;
  }

  public FeatureStore() {
    this(null, null);
  }

  public static String toCsvHeader() {
    List<String> features = Arrays
        .asList("Name", "RelNum", "IntConstNum", "ConstFormulaNum", "VarNum", "ConstExprNum",
            "NotFormulaNum", "InToExprCastNum", "DeclNum", "ExprToIntCastNum", "UnaryExprNum",
            "UnaryIntExprNum", "MultiplicityFormulaNum", "BinaryExprNum", "CompFormulaNum",
            "BinaryFormulaNum", "BinaryIntExprNum", "IntCompFormula", "IfExprNum", "IfIntExprNum",
            "SumExprNum", "QuantFormulaNum", "FixFormulaNum", "ComprehensionNum", "DeclsNum",
            "ProjectExprNum", "RelationPredNum", "NaryExprNum", "NaryIntExprNum", "NaryFormula",           
            "TotalNodeNum", "NodeHeight", "NodeDiameter", "closureNum", "relationinboundsNum");
    return String.join(",", features);
  }

  public String toCsvRow() {
    List<String> features = Arrays.asList(
        /*modelName + "-" + cmdName,
        String.valueOf(relationNum),
        String.valueOf(intConstNum),
        String.valueOf(constFormulaNum),
        String.valueOf(varNum),
        String.valueOf(constExprNum),
        String.valueOf(notFormulaNum),
        String.valueOf(intToExprCastNum),
        String.valueOf(declNum),
        String.valueOf(exprToIntCastNum),
        String.valueOf(unaryExprNum),
        String.valueOf(unaryIntExprNum),
        String.valueOf(multiFormulaNum),
        String.valueOf(binaryExprNum),
        String.valueOf(compFormulaNum),
        String.valueOf(binaryFormulaNum),
        String.valueOf(binaryIntExprNum),
        String.valueOf(intCompFormula),
        String.valueOf(ifExprNum),
        String.valueOf(ifIntExprNum),
        String.valueOf(sumExprNum),
        String.valueOf(quantFormulaNum),
        String.valueOf(fixFormulaNum),
        String.valueOf(comprehensionNum),
        String.valueOf(declsNum),
        String.valueOf(projectExprNum),
        String.valueOf(relationPredNum),
        String.valueOf(naryExprNum),
        String.valueOf(naryIntExprNum),
        String.valueOf(naryFormula),
        String.valueOf(totalNodeNum),
        String.valueOf(nodeHeight),
        String.valueOf(nodeDiameter),
        String.valueOf(closureNum),*/
        String.valueOf(relationinboundsNum)
    );
    return String.join(",", features);
  }

  /**
   * Reset all field except modelName and cmdName.
   */
  public void reset() {
    this.relationNum = 0;
    this.intConstNum = 0;
    this.constFormulaNum = 0;
    this.varNum = 0;
    this.constExprNum = 0;
    this.notFormulaNum = 0;
    this.intToExprCastNum = 0;
    this.declNum = 0;
    this.exprToIntCastNum = 0;
    this.unaryExprNum = 0;
    this.unaryIntExprNum = 0;
    this.multiFormulaNum = 0;
    this.binaryExprNum = 0;
    this.compFormulaNum = 0;
    this.binaryFormulaNum = 0;
    this.binaryIntExprNum = 0;
    this.intCompFormula = 0;
    this.ifExprNum = 0;
    this.ifIntExprNum = 0;
    this.sumExprNum = 0;
    this.quantFormulaNum = 0;
    this.fixFormulaNum = 0;
    this.comprehensionNum = 0;
    this.declsNum = 0;
    this.projectExprNum = 0;
    this.relationPredNum = 0;
    this.naryExprNum = 0;
    this.naryIntExprNum = 0;
    this.naryFormula = 0;
    this.totalNodeNum = 0;
    this.nodeHeight = 0;
    this.nodeDiameter = 0;
    this.closureNum = 0;
    this.relationinboundsNum =0;
  }
}
