package com.deskblast.client;

public class StyleUtil {

    public static String getStyleByPrefix(String prefix, String styles){
        String[] styleArray = styles.split(" ");
        for(String style : styleArray){
            if(style.
                    startsWith(prefix)){
                return style;
            }
        }
        return null;
    }
    
//    
//    public static void main(String[] args){
//        String styles = "left right near-top-22 near-bottom-44";
//        System.out.println(StyleUtil.getStyleByPrefix("near-bottom", styles));
//        System.out.println(StyleUtil.getStyleByPrefix("rig", styles));
//        System.out.println(StyleUtil.getStyleByPrefix("near-to", styles));
//    }
    
}
