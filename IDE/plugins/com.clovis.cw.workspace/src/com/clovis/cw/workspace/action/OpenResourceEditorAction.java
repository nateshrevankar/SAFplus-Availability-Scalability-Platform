/*******************************************************************************
 * ModuleName  : com
 * $File: //depot/dev/main/Andromeda/IDE/plugins/com.clovis.cw.workspace/src/com/clovis/cw/workspace/action/OpenResourceEditorAction.java $
 * $Author: bkpavan $
 * $Date: 2007/01/03 $
 *******************************************************************************/

/*******************************************************************************
 * Description :
 *******************************************************************************/

package com.clovis.cw.workspace.action;

import java.io.File;
import java.net.URL;

import org.eclipse.core.resources.IContainer;
import org.eclipse.core.runtime.Platform;
import org.eclipse.emf.common.notify.NotifyingList;
import org.eclipse.emf.common.util.EList;
import org.eclipse.emf.common.util.URI;
import org.eclipse.jface.action.IAction;
import org.eclipse.ui.IViewActionDelegate;
import org.eclipse.ui.IViewPart;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.IWorkbenchWindowActionDelegate;

import com.clovis.common.utils.ecore.EcoreModels;
import com.clovis.common.utils.ecore.Model;
import com.clovis.cw.data.DataPlugin;
import com.clovis.cw.data.ICWProject;
import com.clovis.cw.genericeditor.GenericEditorInput;
import com.clovis.cw.project.data.ProjectDataModel;
import com.clovis.cw.workspace.WorkspacePlugin;
import com.clovis.cw.workspace.action.CommonMenuAction;

/**
 * Menu to open Clovis Editor.
 * @author shubhada
 *
 */
public class OpenResourceEditorAction extends CommonMenuAction implements
		IViewActionDelegate, IWorkbenchWindowActionDelegate {

	/**
	 * @see org.eclipse.ui.IViewActionDelegate#init(org.eclipse.ui.IViewPart)
	 */
	public void init(IViewPart view) {
	}

	public void dispose() {
		
	}

	public void init(IWorkbenchWindow window) {
		
	}
	public void run(IAction action)
	{
		if (_project != null) {
            openResourceEditor(_project);
        }
	}
	/**
     * return meta-objects list
     * @return EList
     */
    private static EList getMetaObjects()
    {
        try {
            URL url = DataPlugin.getDefault().getBundle().getEntry("/");
            //url = Platform.asLocalURL(url);
            url = Platform.resolve(url);
            String fileName = url.getFile() + "xml" + File.separator
                + "resourcemetaobjects.xmi";
            URI uri = URI.createFileURI(fileName);
            NotifyingList list = (NotifyingList) EcoreModels.read(uri);
            return list;
        } catch (Exception e) {
            e.printStackTrace();
            return null;
        }
    }
	/**
     * @see org.eclipse.ui.IActionDelegate#run(
     * org.eclipse.jface.action.IAction)
     */
    public static void openResourceEditor(IContainer aResource)
    {
        IWorkbenchPage page =
            WorkspacePlugin
                .getDefault()
                .getWorkbench()
                .getActiveWorkbenchWindow()
                .getActivePage();
        if (page != null) {
            IContainer proj = (IContainer) aResource;
            ProjectDataModel pdm = ProjectDataModel.getProjectDataModel(proj);
            String uiFilePath = proj.getLocation().toOSString()
                + File.separator + ICWProject.CW_PROJECT_MODEL_DIR_NAME
                + File.separator + ICWProject.RESOURCE_UI_PROPRTY_FILENAME;
            File uiFile = new File(uiFilePath);
            Model model = pdm.getCAModel().getViewModel();
            
            GenericEditorInput geInput = (GenericEditorInput) pdm.getCAEditorInput();
            if ( geInput == null ) {
            	geInput= new GenericEditorInput(aResource,
                    model, uiFile, getMetaObjects(), 2);
            	pdm.setCAEditorInput(geInput);
            } else {
            	geInput.setModel(model);
            }
            
            try {
                page.openEditor(geInput, ICWProject.RESOURCE_EDITOR_ID);
            } catch (Exception e) {
                WorkspacePlugin.LOG.error("Error in opening Resource Editor.", e);
            }
        }
    }
    public static GenericEditorInput getResourceEditorInput(IContainer aResource)
    {
        IWorkbenchPage page =
            WorkspacePlugin
                .getDefault()
                .getWorkbench()
                .getActiveWorkbenchWindow()
                .getActivePage();
        if (page != null) {
            IContainer proj = (IContainer) aResource;
            ProjectDataModel pdm = ProjectDataModel.getProjectDataModel(proj);
            String uiFilePath = proj.getLocation().toOSString()
                + File.separator + ICWProject.CW_PROJECT_MODEL_DIR_NAME
                + File.separator + ICWProject.RESOURCE_UI_PROPRTY_FILENAME;
            File uiFile = new File(uiFilePath);
            Model model = pdm.getCAModel().getViewModel();
            
            GenericEditorInput geInput = (GenericEditorInput) pdm.getCAEditorInput();
            if ( geInput == null ) {
            	geInput= new GenericEditorInput(aResource,
                    model, uiFile, getMetaObjects(), 2);
            	pdm.setCAEditorInput(geInput);
            } else {
            	geInput.setModel(model);
            }
            try {
                page.openEditor(geInput, ICWProject.RESOURCE_EDITOR_ID);
            } catch (Exception e) {
                WorkspacePlugin.LOG.error("Error in opening Resource Editor.", e);
            }
            return geInput;
        }
        return null;
    }
}
