package edu.mit.csail.sdg.translator.features;

import java.io.PrintWriter;
import java.io.StringWriter;
import java.util.Map;

import kodkod.ast.Formula;
import kodkod.ast.Relation;
import kodkod.instance.Bounds;

public class ExtractKodkodFeatures {

  public static String convert(Formula formula, Map<Relation, Integer>  sorted_map) {
    StringWriter string = new StringWriter();
    PrintWriter file = new PrintWriter(string);
    new ExtractKodkodFeatures(file, formula, sorted_map);
    if (file.checkError()) {
      return ""; // shouldn't happen
    } else {
      return string.toString();
    }
  }

  private ExtractKodkodFeatures(PrintWriter pw, Formula x, Map<Relation, Integer> sorted_map) {
    // Clean the features before collecting
    FeatureExtractor.featureStore.reset();
    x.accept(new FeatureExtractor(sorted_map));
    //pw.printf(FeatureExtractor.featureStore.toCsvRow());
    //System.out.println(FeatureExtractor.featureStore.toCsvRow());
  }
}
