package edu.mit.csail.sdg.translator.features;

import java.util.Arrays;
import java.util.List;

public class BooleanFeatureStore {

  public String modelName;
  public String cmdName;

  public int multiGateNum;
  public int iteGateNum;
  public int notGateNum;
  public int booleanVarNum;

  public int totalNodeNum;
  public int nodeHeight;
  public int nodeDiameter;

  public BooleanFeatureStore(String modelName, String cmdName) {
    this.modelName = modelName;
    this.cmdName = cmdName;
    this.multiGateNum = 0;
    this.iteGateNum = 0;
    this.notGateNum = 0;
    this.booleanVarNum = 0;
    this.totalNodeNum = 0;
    this.nodeHeight = 0;
    this.nodeDiameter = 0;
  }

  public BooleanFeatureStore() {
    this(null, null);
  }

  public static String toCsvHeader() {
    List<String> features = Arrays
        .asList("Name", "MultiGateNum", "ITEGateNum", "NotGateNum", "BooleanVarNum", "TotalNodeNum",
            "NodeHeight", "NodeDiameter");
    return String.join(",", features);
  }

  public String toCsvRow() {
    List<String> features = Arrays.asList(
        modelName + "-" + cmdName,
        String.valueOf(multiGateNum),
        String.valueOf(iteGateNum),
        String.valueOf(notGateNum),
        String.valueOf(booleanVarNum),
        String.valueOf(totalNodeNum),
        String.valueOf(nodeHeight),
        String.valueOf(nodeDiameter)
    );
    return String.join(",", features);
  }

  public void reset() {
    this.multiGateNum = 0;
    this.iteGateNum = 0;
    this.notGateNum = 0;
    this.booleanVarNum = 0;
    this.totalNodeNum = 0;
    this.nodeHeight = 0;
    this.nodeDiameter = 0;
  }
}
