.//=====================================================================
.//
.// File:      $RCSfile: create_adapters.inc,v $
.// Version:   $Revision: 1.31 $
.// Modified:  $Date: 2013/01/17 03:31:00 $
.//
.// (c) Copyright 2006-2014 by Mentor Graphics Corp. All rights reserved.
.//
.//=====================================================================
.// Licensed under the Apache License, Version 2.0 (the "License"); you may not
.// use this file except in compliance with the License.  You may obtain a copy
.// of the License at
.//
.//      http://www.apache.org/licenses/LICENSE-2.0
.//
.// Unless required by applicable law or agreed to in writing, software
.// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
.// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.   See the
.// License for the specific language governing permissions and limitations under
.// the License.
.//=====================================================================
.//
.// This file is responsible for creating the adapters that decouple the
.// client model objects from the tree viewer and the hierarchy it 
.// imposes.
.//
.function create_adapters
  .param inst_ref tree_node
  .param string node_name
  .param string path
  .//
  .assign adapter_name = "${node_name}Adapter"
  .assign attr_result = adapter_name
  .select any this_class from instances of O_OBJ where (selected.Key_Lett == tree_node.Key_Lett)
  .invoke result = get_class_name(this_class)
  .assign this_class_name = "${result.body}"
package org.xtuml.bp.ui.explorer.adapters;
//======================================================================
//
// File: ${path}/adapters/${adapter_name}.java
//
// WARNING:      Do not edit this generated file
// Generated by: ${info.arch_file_name}
// Version:      $$Revision: 1.31 $$
//
// (c) Copyright 2006-2014 by Mentor Graphics Corp.  All rights reserved.
//
//======================================================================
//
// This class is responsible for decoupling the client model entity
// class ${this_class_name} from the tree viewer and the hierarchy
// it imposes.
//
import java.util.ArrayList;
import java.util.List;

.if (tree_node.Key_Lett == "S_SYS")
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IFolder;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.NullProgressMonitor;
import org.xtuml.bp.ui.explorer.ExplorerPlugin;
.end if
import org.eclipse.jface.viewers.ITreeContentProvider;
import org.eclipse.jface.viewers.Viewer;
import org.xtuml.bp.core.*;

import org.xtuml.bp.ui.explorer.*;
  .select many child_specs related by tree_node->T_TPS[R1000]
  .for each spec in child_specs
    .select one child related by spec->T_TNS[R1001]
    .if ( child.Sorted )
      .select any child_class from instances of O_OBJ where (selected.Key_Lett == child.Key_Lett)
      .invoke gcn = get_class_name(child_class)
      .assign child_class_name = "${gcn.body}"
import org.xtuml.bp.core.sorter.${child_class_name}Sorter;
    .end if
  .end for
/**
 * This file adapts the meta-model entity; '${this_class_name}'
 * so that it works with the Eclipse JFace user interface components.
 * <p>
 * Do not edit this class, it was created using the Mentor 
 * Graphics MC-Java code generator product.
 * </p>
 */
public  class ${adapter_name} implements ITreeContentProvider {
  static ${adapter_name} $l{adapter_name} = null;
  /**
   * Returns the adapters singleton instance. If this
   * is the first time, the instance is created.
   */
  public static ${adapter_name} getInstance() {
    if ($l{adapter_name} == null) {
      $l{adapter_name} = new ${adapter_name}();
    }
    return $l{adapter_name};
  }
  /**
   * @see IContentProvider#inputChanged(Viewer, Object, Object)
   * Called when the tree's input has been changed
   */
  public void inputChanged(Viewer viewer, Object oldInput, Object newInput) {
    // Nothing to do
  }
  /**
   * @see IContentProvider#dispose()
   * Called when this viewer is no longer needed
   */
  public void dispose() {
    // Nothing to dispose
  }
  /**
   * @see ITreeContentProvider#getParent(Object)
   * Returns the parent of this node
   */
  public Object getParent(Object arg) {
  .select many spec_set related by tree_node->T_TPS[R1001] where (selected.ParentOrder == "first")
  .for each spec in spec_set
    .assign result_var = "result${info.unique_num}"
    .select one parent related by spec->T_TNS[R1000]
    .invoke rel_nav = generate_forward_rel_chain_nav( spec.ParentChildRelChain, parent.Key_Lett, this_class_name, "arg", result_var ) 
${rel_nav.body}\
    if (${result_var} != null) {
      return ${result_var};
    }
    .end for
  .select many spec_set related by tree_node->T_TPS[R1001] where (selected.ParentOrder != "first")
  .for each spec in spec_set
    .assign result_var = "result${info.unique_num}"
    .select one parent related by spec->T_TNS[R1000]
    .invoke rel_nav = generate_forward_rel_chain_nav( spec.ParentChildRelChain, parent.Key_Lett, this_class_name, "arg", result_var ) 
${rel_nav.body}\
    if (${result_var} != null) {
      return ${result_var};
    }
    .end for
.if (tree_node.Key_Lett == "S_SYS")
	return Ooaofooa.getDefaultInstance();
.else
    return null;
.end if
  }
  /**
   * @see IStructuredContentProvider#getElements(Object)
   * Returns the elements below this node
   */
  public Object[] getElements(Object arg) {
  	return getChildren(arg);
  }

  /**
   * @see ITreeContentProvider#getChildren(Object)
   * Returns the children of this node
   */
  public Object[] getChildren(Object arg) {
    int resultSize = 0;
  .//
  .// We need to return a set of instances for each item that is specified
  .// as a child of the element represented by this adapter.
  .//
  .assign childCount = 0
  .for each spec in child_specs
    .assign childCount = childCount + 1
    .select one child related by spec->T_TNS[R1001]
    .assign relNumber = "$tcf_rel{spec.ParentChildRelChain}"
    .assign varName = "$lr{child.CategoryName}_${relNumber}_${childCount}"
    .invoke rel_nav = generate_backward_rel_chain_nav(tree_node, spec.ParentChildRelChain, "many", "arg", "${varName}")
${rel_nav.body}\
    .if ( not child.Sorted )
      .assign alpha_sort = false
      .select any child_class from instances of O_OBJ where (selected.Key_Lett == child.Key_Lett)
      .//
      .select one attr related by child_class->O_ATTR[R102] where ( selected.Name == "Name" )
      .if ( not_empty attr )
        .assign alpha_sort = true
      .else
        .select one op related by child_class->O_TFR[R115] where ( selected.Name == "get_name" )
        .if ( not_empty op )
          .assign alpha_sort = true
        .end if
      .end if
      .//
      .if ( alpha_sort )
    	        ModelContentProvider.sort(${varName});
      .end if
    .end if
    resultSize += ${varName}.length;
  .end for
  
  .invoke is_compRoot = is_componentRootElementKL(tree_node.Key_Lett) 
    Object [] result = new Object [resultSize];
  .if (not_empty child_specs)
	    int count = 0;  
  .end if
  .assign childCount = 0
  .for each spec in child_specs
    .assign childCount = childCount + 1
    .select one child related by spec->T_TNS[R1001]
    .assign relNumber = "$tcf_rel{spec.ParentChildRelChain}"
    .assign varName = "$lr{child.CategoryName}_${relNumber}_${childCount}"
    .if ( child.Sorted )
      .select any child_class from instances of O_OBJ where (selected.Key_Lett == child.Key_Lett)
      .invoke gcn = get_class_name(child_class)
      .assign child_class_name = "${gcn.body}"
    ${child_class_name}Sorter sorter${child.Key_Lett} = new ${child_class_name}Sorter();
    sorter${child.Key_Lett}.sort(${varName});
    .end if
    for (int i = 0 ; i < ${varName}.length ; i++) {
    .if(child.occursInMultipleLocations)
      result[count] = new MultipleOccurrenceElement(${varName}[i]);
    .else
      result[count] = ${varName}[i];
    .end if
      count++;
    }
  .end for
  return result;
  }
  /**
   * @see ITreeContentProvider#hasChildren(Object)
   * Returns true if this node has any children
   */
  public boolean hasChildren(Object arg) {
  .assign childCount = 0
  .for each spec in child_specs
    .assign childCount = childCount + 1
    .select one child related by spec->T_TNS[R1001]
    .assign relNumber = "$tcf_rel{spec.ParentChildRelChain}"
    .assign varName = "$lr{child.CategoryName}_${relNumber}_${childCount}"
    .invoke rel_nav = generate_backward_rel_chain_nav(tree_node, spec.ParentChildRelChain, "many", "arg", "${varName}")
         ${rel_nav.body}
         if (${varName}.length > 0) return true;  
  .end for
    return false;
  }
}
.end function