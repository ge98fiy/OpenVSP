//
// This file is released under the terms of the NASA Open Source Agreement (NOSA)
// version 1.3 as detailed in the LICENSE file which accompanies this software.
//

// CfdMeshScreen.cpp: implementation of the CfdMeshScreen class.
//
//////////////////////////////////////////////////////////////////////

#include "CfdMeshScreen.h"
#include "CfdMeshMgr.h"
#include "StreamUtil.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CfdMeshScreen::CfdMeshScreen( ScreenMgr* mgr ) : TabScreen( mgr, 415, 622, "CFD Mesh", 150 )
{
    m_Vehicle = m_ScreenMgr->GetVehiclePtr();

    m_FLTK_Window->callback( staticCloseCB, this );

    //=== Create Tabs ===//
    CreateGlobalTab();
    CreateDisplayTab();
    CreateOutputTab();
    CreateSourcesTab();
    CreateDomainTab();
    CreateWakesTab();

    //=== Create Console Area ===//
    m_ConsoleLayout.SetGroupAndScreen( m_FLTK_Window, this );

    m_ConsoleLayout.AddY( m_ConsoleLayout.GetRemainY()
                        - 7 * m_ConsoleLayout.GetStdHeight()
                        - 2.0 * m_ConsoleLayout.GetGapHeight() );

    m_ConsoleLayout.AddYGap();
    m_ConsoleLayout.AddX(5);

    m_ConsoleLayout.AddSubGroupLayout( m_BorderConsoleLayout, m_ConsoleLayout.GetRemainX() - 5.0,
                                       m_ConsoleLayout.GetRemainY() - 5.0);

    m_ConsoleDisplay = m_BorderConsoleLayout.AddFlTextDisplay( 115 );
    m_ConsoleBuffer = new Fl_Text_Buffer;
    m_ConsoleDisplay->buffer( m_ConsoleBuffer );

    m_BorderConsoleLayout.AddYGap();

    m_BorderConsoleLayout.AddButton(m_MeshAndExport, "Mesh and Export");
}

CfdMeshScreen::~CfdMeshScreen()
{
}

void CfdMeshScreen::CreateGlobalTab()
{
    Fl_Group* globalTab = AddTab( "Global" );
    Fl_Group* globalTabGroup = AddSubGroup( globalTab, 5 );

    m_GlobalTabLayout.SetGroupAndScreen( globalTabGroup, this );

    //=== GLOBAL TAB INIT ===//

    m_GlobalTabLayout.AddDividerBox("Global Mesh Control");

    m_GlobalTabLayout.AddYGap();

    m_GlobalTabLayout.SetButtonWidth(175.0);
    m_GlobalTabLayout.AddSlider(m_MaxEdgeLen, "Max Edge Len", 1.0, "%7.5f");
    m_GlobalTabLayout.AddSlider(m_MinEdgeLen, "Min Edge Len", 1.0, "%7.5f");
    m_GlobalTabLayout.AddSlider(m_MaxGap, "Max Gap", 1.0, "%7.5f");
    m_GlobalTabLayout.AddSlider(m_NumCircleSegments, "Num Circle Segments", 100.0, "%7.5f");
    m_GlobalTabLayout.AddSlider(m_GrowthRatio, "Growth Ratio", 2.0, "%7.5f");

    m_GlobalTabLayout.AddYGap();
    m_GlobalTabLayout.AddButton(m_Rig3dGrowthLimit, "Rigorous 3D Growth Limiting");
    m_GlobalTabLayout.AddYGap();
    m_GlobalTabLayout.AddDividerBox("Global Source Control");
    m_GlobalTabLayout.AddYGap();

    m_GlobalTabLayout.SetFitWidthFlag( false );
    m_GlobalTabLayout.SetSameLineFlag( true );

    m_GlobalTabLayout.SetButtonWidth(20.0);
    m_GlobalTabLayout.AddButton(m_GlobSrcAdjustLenLftLft, "<<");
    m_GlobalTabLayout.AddButton(m_GlobSrcAdjustLenLft, "<");
    m_GlobalTabLayout.AddLabel("Adjust Len", 100.0);
    m_GlobalTabLayout.AddButton(m_GlobSrcAdjustLenRht, ">");
    m_GlobalTabLayout.AddButton(m_GlobSrcAdjustLenRhtRht, ">>");
    m_GlobalTabLayout.AddX(45.0);
    m_GlobalTabLayout.AddButton(m_GlobSrcAdjustRadLftLft, "<<");
    m_GlobalTabLayout.AddButton(m_GlobSrcAdjustRadLft, "<");
    m_GlobalTabLayout.AddLabel("Adjust Rad", 100.0);
    m_GlobalTabLayout.AddButton(m_GlobSrcAdjustRadRht, ">");
    m_GlobalTabLayout.AddButton(m_GlobSrcAdjustRadRhtRht, ">>");
    m_GlobalTabLayout.ForceNewLine();
    m_GlobalTabLayout.AddYGap();

    m_GlobalTabLayout.SetFitWidthFlag( true );
    m_GlobalTabLayout.SetSameLineFlag( false );

    m_GlobalTabLayout.AddDividerBox("Geometry Control");
    m_GlobalTabLayout.AddYGap();
    m_GlobalTabLayout.AddButton(m_IntersectSubsurfaces, "Intersect Subsurfaces");
    m_GlobalTabLayout.AddYGap();

    m_GlobalTabLayout.SetChoiceButtonWidth(m_GlobalTabLayout.GetRemainX() / 2.0);
    m_GlobalTabLayout.AddChoice(m_UseSet, "Use Set");

    globalTab->show();
}

void CfdMeshScreen::CreateDisplayTab()
{
    Fl_Group* displayTab = AddTab( "Display" );
    Fl_Group* displayTabGroup = AddSubGroup( displayTab, 5 );

    m_DisplayTabLayout.SetGroupAndScreen( displayTabGroup, this );

    m_DisplayTabLayout.AddYGap();
    m_DisplayTabLayout.AddButton(m_ShowSourcesAndWakePreview, "Show Sources and Wake Preview");
    m_DisplayTabLayout.AddYGap();
    m_DisplayTabLayout.AddButton(m_ShowFarFieldPreview, "Show Far Field Preview");
    m_DisplayTabLayout.AddYGap();
    m_DisplayTabLayout.AddButton(m_ShowMesh, "Show Mesh");
    m_DisplayTabLayout.AddYGap();
    m_DisplayTabLayout.AddButton(m_ShowWake, "Show Wake");
    m_DisplayTabLayout.AddYGap();
    m_DisplayTabLayout.AddButton(m_ShowSymmetryPlane, "Show Symmetry Plane");
    m_DisplayTabLayout.AddYGap();
    m_DisplayTabLayout.AddButton(m_ShowFarField, "Show Far Field");
    m_DisplayTabLayout.AddYGap();
    m_DisplayTabLayout.AddButton(m_ShowBadEdgesAndTriangles, "Show Bad Edges and Triangles");
    m_DisplayTabLayout.AddYGap();
    m_DisplayTabLayout.AddButton(m_ColorTags, "Color Tags");

    displayTab->show();
}

void CfdMeshScreen::CreateOutputTab()
{
    Fl_Group* outputTab = AddTab( "Output" );
    Fl_Group* outputTabGroup = AddSubGroup( outputTab, 5 );

    m_OutputTabLayout.SetGroupAndScreen( outputTabGroup, this );

    m_OutputTabLayout.AddDividerBox("Export File Names");
    m_OutputTabLayout.AddYGap();

    m_OutputTabLayout.SetFitWidthFlag( false );
    m_OutputTabLayout.SetSameLineFlag( true );

    m_OutputTabLayout.SetButtonWidth(50);
    m_OutputTabLayout.SetInputWidth(305);
    m_OutputTabLayout.AddButton(m_StlFile, ".stl");
    m_OutputTabLayout.AddOutput(m_StlOutput);
    m_OutputTabLayout.AddButton(m_SelectStlFile, "...");
    m_OutputTabLayout.ForceNewLine();

    m_OutputTabLayout.SetFitWidthFlag( true );
    m_OutputTabLayout.AddButton(m_TaggedMultiSolid, "Tagged Multi Sold STL (Non-Standard)");
    m_OutputTabLayout.SetFitWidthFlag( false );
    m_OutputTabLayout.ForceNewLine();
    m_OutputTabLayout.AddYGap();

    m_OutputTabLayout.AddButton(m_PolyFile, ".poly");
    m_OutputTabLayout.AddOutput(m_PolyOutput);
    m_OutputTabLayout.AddButton(m_SelectPolyFile, "...");
    m_OutputTabLayout.ForceNewLine();
    m_OutputTabLayout.AddButton(m_TriFile, ".tri");
    m_OutputTabLayout.AddOutput(m_TriOutput);
    m_OutputTabLayout.AddButton(m_SelectTriFile, "...");
    m_OutputTabLayout.ForceNewLine();
    m_OutputTabLayout.AddButton(m_ObjFile, ".obj");
    m_OutputTabLayout.AddOutput(m_ObjOutput);
    m_OutputTabLayout.AddButton(m_SelectObjFile, "...");
    m_OutputTabLayout.ForceNewLine();
    m_OutputTabLayout.AddButton(m_MshFile, ".msh");
    m_OutputTabLayout.AddOutput(m_MshOutput);
    m_OutputTabLayout.AddButton(m_SelectMshFile, "...");
    m_OutputTabLayout.ForceNewLine();
    m_OutputTabLayout.AddYGap();

    m_OutputTabLayout.SetFitWidthFlag( true );
    m_OutputTabLayout.AddDividerBox("NASCART");
    m_OutputTabLayout.ForceNewLine();
    m_OutputTabLayout.SetFitWidthFlag( false );

    m_OutputTabLayout.AddButton(m_DatFile, ".dat");
    m_OutputTabLayout.AddOutput(m_DatOutput);
    m_OutputTabLayout.AddButton(m_SelectDatFile, "...");
    m_OutputTabLayout.ForceNewLine();
    m_OutputTabLayout.AddButton(m_KeyFile, ".key");
    m_OutputTabLayout.AddOutput(m_KeyOutput);
    m_OutputTabLayout.AddButton(m_SelectKeyFile, "...");
    m_OutputTabLayout.ForceNewLine();
    m_OutputTabLayout.AddYGap();

    m_OutputTabLayout.SetFitWidthFlag( true );
    m_OutputTabLayout.AddDividerBox("Surfaces and Intersection Curves");
    m_OutputTabLayout.ForceNewLine();
    m_OutputTabLayout.SetFitWidthFlag( false );

    m_OutputTabLayout.AddButton(m_SrfFile, ".srf");
    m_OutputTabLayout.AddOutput(m_SrfOutput);
    m_OutputTabLayout.AddButton(m_SelectSrfFile, "...");
    m_OutputTabLayout.ForceNewLine();
    m_OutputTabLayout.AddYGap();

    m_OutputTabLayout.SetFitWidthFlag( true );
    m_OutputTabLayout.AddDividerBox("Surface and Subsurface Key File");
    m_OutputTabLayout.ForceNewLine();
    m_OutputTabLayout.SetFitWidthFlag( false );

    m_OutputTabLayout.AddButton(m_TkeyFile, ".tkey");
    m_OutputTabLayout.AddOutput(m_TkeyOutput);
    m_OutputTabLayout.AddButton(m_SelectTkeyFile, "...");

    outputTab->show();
}

void CfdMeshScreen::CreateSourcesTab()
{
    Fl_Group* sourcesTab = AddTab( "Sources" );
    Fl_Group* sourcesTabGroup = AddSubGroup( sourcesTab, 5 );

    m_SourcesTabLayout.SetGroupAndScreen( sourcesTabGroup, this );

    m_SourcesTabLayout.AddYGap();

    m_SourcesTabLayout.SetFitWidthFlag( true );
    m_SourcesTabLayout.SetSameLineFlag( true );

    m_SourcesTabLayout.SetChoiceButtonWidth(100);
    m_SourcesTabLayout.AddChoice(m_SourcesSelectComp, "Select Comp", 140);
    m_SourcesTabLayout.SetFitWidthFlag( false );
    m_SourcesTabLayout.SetButtonWidth(140);
    m_SourcesTabLayout.AddButton(m_AddDefaultSources, "Add Default Sources");
    m_SourcesTabLayout.ForceNewLine();
    m_SourcesTabLayout.AddYGap();

    m_SourcesTabLayout.SetFitWidthFlag( true );
    m_SourcesTabLayout.AddChoice(m_SourcesSelectSurface, "Select Surface");
    m_SourcesTabLayout.ForceNewLine();
    m_SourcesTabLayout.AddYGap();

    m_SourcesTabLayout.AddDividerBox("Sources");
    m_SourcesTabLayout.ForceNewLine();

    m_SourcesTabLayout.AddSubGroupLayout(m_SourcesLeft, m_SourcesTabLayout.GetRemainX() / 2.0 - 5.0, 300);
    m_SourcesTabLayout.AddX(m_SourcesTabLayout.GetRemainX() / 2.0 + 5.0);
    m_SourcesTabLayout.AddSubGroupLayout(m_SourcesRight, m_SourcesTabLayout.GetRemainX() - 5.0, 300);

    m_SourcesLeft.SetFitWidthFlag( true );
    m_SourcesLeft.SetSameLineFlag( false );

    m_SourcesLeft.SetChoiceButtonWidth(50);
    m_SourcesType.AddItem("Point");
    m_SourcesType.AddItem("Line");
    m_SourcesType.AddItem("Box");
    m_SourcesLeft.AddChoice(m_SourcesType, "Type");
    m_SourcesLeft.AddYGap();
    m_SourcesLeft.AddButton(m_AddSource, "Add Source");
    m_SourcesLeft.AddYGap();
    m_SourcesLeft.AddButton(m_DeleteSource, "Delete Source");

    m_SourceBrowser = m_SourcesRight.AddFlBrowser(75);
    m_SourceBrowser->callback( staticScreenCB, this );

    m_SourcesTabLayout.ForceNewLine();
    m_SourcesTabLayout.ForceNewLine();
    m_SourcesTabLayout.ForceNewLine();
    m_SourcesTabLayout.ForceNewLine();

    m_SourcesTabLayout.AddDividerBox("Edit Source");

    m_SourcesTabLayout.ForceNewLine();

    m_SourcesTabLayout.AddInput(m_SourceName, "Name:");

    m_SourcesTabLayout.ForceNewLine();
    m_SourcesTabLayout.AddYGap();

    m_SourcesTabLayout.AddSubGroupLayout(m_UWPosition1, m_SourcesTabLayout.GetRemainX() / 2.0 - 5.0, 400);
    m_SourcesTabLayout.AddX(m_SourcesTabLayout.GetRemainX() / 2.0 + 5.0);
    m_SourcesTabLayout.AddSubGroupLayout(m_UWPosition2, m_SourcesTabLayout.GetRemainX() - 5.0, 400);

    m_UWPosition1.AddDividerBox("UW Position 1");
    m_UWPosition1.AddYGap();
    m_UWPosition1.SetButtonWidth(40);
    m_UWPosition1.AddSlider(m_SourceU1, "U", 1.0, "%7.3f");
    m_UWPosition1.AddSlider(m_SourceW1, "W", 1.0, "%7.3f");
    m_UWPosition1.AddYGap();
    m_UWPosition1.AddSlider(m_SourceRad1, "Rad", 1.0, "%7.2g");
    m_UWPosition1.AddSlider(m_SourceLen1, "Len", 1.0, "%7.2g");

    m_UWPosition2.AddDividerBox("UW Position 2");
    m_UWPosition2.AddYGap();
    m_UWPosition2.SetButtonWidth(40);
    m_UWPosition2.AddSlider(m_SourceU2, "U", 1.0, "%7.3f");
    m_UWPosition2.AddSlider(m_SourceW2, "W", 1.0, "%7.3f");
    m_UWPosition2.AddYGap();
    m_UWPosition2.AddSlider(m_SourceRad2, "Rad", 1.0, "%7.2g");
    m_UWPosition2.AddSlider(m_SourceLen2, "Len", 1.0, "%7.2g");

    sourcesTab->show();
}

void CfdMeshScreen::CreateDomainTab()
{
    Fl_Group* domainTab = AddTab( "Domain" );
    Fl_Group* domainTabGroup = AddSubGroup( domainTab, 5 );

    m_DomainTabLayout.SetGroupAndScreen( domainTabGroup, this );

    m_DomainTabLayout.AddYGap();

    m_DomainTabLayout.AddButton(m_GenerateHalfMesh, "Generate Half Mesh");
    m_DomainTabLayout.AddYGap();
    m_DomainTabLayout.AddButton(m_GenerateFarFieldMesh, "Generate Far Field Mesh");
    m_DomainTabLayout.AddYGap();

    m_DomainTabLayout.AddSubGroupLayout(m_FarParametersLayout, m_DomainTabLayout.GetRemainX(),
                                        m_DomainTabLayout.GetRemainY());

    m_FarParametersLayout.AddDividerBox("Far Field Mesh Control");
    m_FarParametersLayout.AddYGap();
    m_FarParametersLayout.SetButtonWidth(m_DomainTabLayout.GetRemainX() / 2.0);
    m_FarParametersLayout.AddSlider(m_DomainMaxEdgeLen, "Max Edge Len", 1.0, "%7.5f");
    m_FarParametersLayout.AddSlider(m_DomainMaxGap, "Max Gap", 1.0, "%7.5f");
    m_FarParametersLayout.AddSlider(m_DomainNumCircleSegments, "Num Circle Segments", 100.0, "%7.5f");
    m_FarParametersLayout.AddYGap();

    //=== Far Field Type Area ===//
    m_FarParametersLayout.AddDividerBox("Far Field Type");
    m_FarParametersLayout.AddYGap();

    m_FarParametersLayout.SetFitWidthFlag( false );
    m_FarParametersLayout.SetSameLineFlag( true );

    m_FarParametersLayout.SetButtonWidth(100);
    m_FarParametersLayout.AddButton(m_FarFieldTypeComponent, "Component:", -1);
    m_FarParametersLayout.SetFitWidthFlag( true );
    m_FarParametersLayout.SetChoiceButtonWidth(0);
    m_FarParametersLayout.AddChoice(m_ComponentChoice, "", 100);

    m_FarParametersLayout.ForceNewLine();
    m_FarParametersLayout.SetFitWidthFlag( false );
    m_FarParametersLayout.AddButton(m_FarFieldTypeBox, "Box", 1);
    m_FarFieldTypeBox.GetFlButton()->value( 1 );

    m_FarParametersLayout.ForceNewLine();

    m_FarParametersLayout.AddSubGroupLayout(m_FarBoxLayout, m_FarParametersLayout.GetRemainX(),
                                            m_FarParametersLayout.GetRemainY());

    //=== Symmetry Plane Splitting Toggle Button ===//
    m_FarBoxLayout.AddButton(m_SymPlaneSplit, "Symmetry Plane Splitting");
    m_FarBoxLayout.AddYGap();

    //=== Size Area ===//
    m_FarBoxLayout.SetFitWidthFlag( true );
    m_FarBoxLayout.SetSameLineFlag( true );

    m_FarBoxLayout.AddDividerBox("Size", 120);

    m_FarBoxLayout.SetFitWidthFlag( false );

    m_FarBoxLayout.SetButtonWidth(60.0);
    m_FarBoxLayout.AddButton(m_DomainRel, "Rel");
    m_FarBoxLayout.AddButton(m_DomainAbs, "Abs");
    m_DomainRel.GetFlButton()->value( 1 );

    m_FarBoxLayout.ForceNewLine();

    m_FarBoxLayout.SetFitWidthFlag( true );
    m_FarBoxLayout.SetSameLineFlag( false );

    m_FarBoxLayout.AddYGap();
    m_FarBoxLayout.SetButtonWidth(m_DomainTabLayout.GetRemainX() / 3.0);
    m_FarBoxLayout.AddSlider(m_DomainLength, "Length", 10.0, "%7.5f");
    m_FarBoxLayout.AddSlider(m_DomainWidth, "Width", 10.0, "%7.5f");
    m_FarBoxLayout.AddSlider(m_DomainHeight, "Height", 10.0, "%7.5f");
    m_FarBoxLayout.AddYGap();

    //=== Location Area ===//
    m_FarBoxLayout.SetFitWidthFlag( true );
    m_FarBoxLayout.SetSameLineFlag( true );

    m_FarBoxLayout.AddDividerBox("Location", 120);

    m_FarBoxLayout.SetFitWidthFlag( false );

    m_FarBoxLayout.SetButtonWidth(60.0);
    m_FarBoxLayout.AddButton(m_DomainCen, "Cen");
    m_FarBoxLayout.AddButton(m_DomainMan, "Man");
    m_DomainCen.GetFlButton()->value( 1 );

    m_FarBoxLayout.ForceNewLine();

    m_FarBoxLayout.SetFitWidthFlag( true );
    m_FarBoxLayout.SetSameLineFlag( false );

    m_FarBoxLayout.AddYGap();

    m_FarBoxLayout.AddSubGroupLayout(m_FarXYZLocationLayout, m_FarBoxLayout.GetRemainX(),
                                     m_FarBoxLayout.GetRemainY());

    m_FarXYZLocationLayout.SetButtonWidth(m_DomainTabLayout.GetRemainX() / 3.0);
    m_FarXYZLocationLayout.AddSlider(m_DomainXLoc, "X Loc", 5.0, "%7.5f");
    m_FarXYZLocationLayout.AddSlider(m_DomainYLoc, "Y Loc", 5.0, "%7.5f");
    m_FarXYZLocationLayout.AddSlider(m_DomainZLoc, "Z Loc", 5.0, "%7.5f");
    m_FarXYZLocationLayout.AddYGap();

    domainTab->show();
}

void CfdMeshScreen::CreateWakesTab()
{
    Fl_Group* wakesTab = AddTab( "Wakes" );
    Fl_Group* wakesTabGroup = AddSubGroup( wakesTab, 5 );

    m_WakesTabLayout.SetGroupAndScreen( wakesTabGroup, this );

    m_WakesTabLayout.AddYGap();

    m_WakesTabLayout.SetButtonWidth(175.0);
    m_WakesTabLayout.AddSlider(m_ScaleWake, "Scale Wake", 10.0, "%7.5f");
    m_WakesTabLayout.AddYGap();
    m_WakesTabLayout.AddSlider(m_WakeAngle, "Wake Angle", 10.0, "%7.5f");

    m_WakesTabLayout.ForceNewLine();

    m_WakesTabLayout.SetFitWidthFlag( true );
    m_WakesTabLayout.SetSameLineFlag( true );

    m_WakesTabLayout.SetChoiceButtonWidth(100);
    m_WakesTabLayout.AddChoice(m_Comp, "Comp", 100);

    m_WakesTabLayout.SetFitWidthFlag(false);
    m_WakesTabLayout.SetButtonWidth(100);
    m_WakesTabLayout.AddButton(m_AddWake, "Add Wake");

    wakesTab->show();
}

bool CfdMeshScreen::Update()
{
    LoadSetChoice();

    CfdMeshMgr.UpdateSourcesAndWakes();
    CfdMeshMgr.UpdateDomain();

    //==== Load Geom Choice ====//
    m_GeomVec = m_Vehicle->GetGeomVec();

    m_SourcesSelectComp.ClearItems();
    m_SourcesSelectSurface.ClearItems();
    m_ComponentChoice.ClearItems();
    m_Comp.ClearItems();

    map< string, int > compIDMap;
    map< string, int > wingCompIDMap;
    m_WingGeomVec.clear();


    int iwing = 0;
    for ( int i = 0 ; i < ( int )m_GeomVec.size() ; ++i )
    {
        char str[256];
        Geom* g = m_Vehicle->FindGeom( m_GeomVec[i] );
        if ( g )
        {
            sprintf( str, "%d_%s", i, g->GetName().c_str() );
            m_SourcesSelectComp.AddItem( str );
            if( g->HasWingTypeSurfs() )
            {
                m_Comp.AddItem( str );
                wingCompIDMap[ m_GeomVec[i] ] = iwing++;
                m_WingGeomVec.push_back( m_GeomVec[i] );
            }
            m_ComponentChoice.AddItem( str );
            compIDMap[ m_GeomVec[i] ] = i;
        }
    }

    //===== Set Current Source and component for Sources =====//
    string currSourceGeomID = CfdMeshMgr.GetCurrSourceGeomID();
    if( currSourceGeomID.length() == 0 && m_GeomVec.size() > 0 )
    {
        // Handle case default case.
        currSourceGeomID = m_GeomVec[0];
        CfdMeshMgr.SetCurrSourceGeomID( currSourceGeomID );
    }
    Geom* currGeom = m_Vehicle->FindGeom( currSourceGeomID );
    m_SourcesSelectComp.SetVal( compIDMap[ currSourceGeomID ] );

    //===== Set FarGeomID and Far Component Selection for Domain Tab =====//
    string farGeomID = CfdMeshMgr.GetCfdSettingsPtr()->GetFarGeomID();
    if( farGeomID.length() == 0 && m_GeomVec.size() > 0 )
    {
        // Handle case default case.
        farGeomID = m_GeomVec[0];
        CfdMeshMgr.GetCfdSettingsPtr()->SetFarGeomID( farGeomID );
    }
    m_ComponentChoice.SetVal( compIDMap[ farGeomID ] );

    //===== Set WakeGeomID and wake component selection for wake tab =====//
    string wakeGeomID = CfdMeshMgr.GetWakeGeomID();
    if( wakeGeomID.length() == 0 && m_WingGeomVec.size() > 0 )
    {
        // Handle case default case.
        wakeGeomID = m_WingGeomVec[0];
        CfdMeshMgr.SetWakeGeomID( wakeGeomID );
    }
    Geom* wakeGeom = m_Vehicle->FindGeom( wakeGeomID );
    m_Comp.SetVal( wingCompIDMap[ wakeGeomID ] );

    //===== Update Sources =====//
    BaseSource* source = CfdMeshMgr.GetCurrSource();

    //==== Load Up Source Browser ====//
    int currSourceID = -1;

    m_SourceBrowser->clear();
    m_SourcesSelectSurface.ClearItems();

    if( currGeom )
    {
        vector< BaseSource* > sVec = currGeom->GetCfdMeshMainSourceVec();
        for ( int i = 0 ; i < ( int )sVec.size() ; ++i )
        {
            if ( source == sVec[i] )
            {
                currSourceID = i;
            }
            m_SourceBrowser->add( sVec[i]->GetName().c_str() );
        }
        if ( currSourceID >= 0 && currSourceID < ( int )sVec.size() )
        {
            m_SourceBrowser->select( currSourceID + 1 );
        }

        int nmain = currGeom->GetNumMainSurfs();
        for ( int i = 0; i < nmain; ++i )
        {
            char str[256];
            sprintf( str, "Surf_%d", i );
            m_SourcesSelectSurface.AddItem( str );
        }
        int currMainSurfID = CfdMeshMgr.GetCurrMainSurfIndx();
        if( currMainSurfID >= 0 && currMainSurfID < nmain )
        {
            m_SourcesSelectSurface.SetVal( currMainSurfID );
        }
    }
    m_SourcesSelectSurface.UpdateItems();

    UpdateGlobalTab();
    UpdateDisplayTab();
    UpdateOutputTab();
    UpdateSourcesTab(source);
    UpdateDomainTab();
    UpdateWakesTab();

    m_FLTK_Window->redraw();

    return false;
}

void CfdMeshScreen::UpdateGlobalTab()
{
    //===== Global Mesh Control =====//
    m_MaxEdgeLen.Update( CfdMeshMgr.GetGridDensityPtr()->m_BaseLen.GetID() );
    m_MinEdgeLen.Update( CfdMeshMgr.GetGridDensityPtr()->m_MinLen.GetID() );
    m_MaxGap.Update( CfdMeshMgr.GetGridDensityPtr()->m_MaxGap.GetID() );
    m_NumCircleSegments.Update( CfdMeshMgr.GetGridDensityPtr()->m_NCircSeg.GetID() );
    m_GrowthRatio.Update( CfdMeshMgr.GetGridDensityPtr()->m_GrowRatio.GetID() );
    m_Rig3dGrowthLimit.Update( CfdMeshMgr.GetGridDensityPtr()->m_RigorLimit.GetID() );

    //===== Geometry Control =====//
    m_IntersectSubsurfaces.Update( CfdMeshMgr.GetCfdSettingsPtr()->m_IntersectSubSurfs.GetID() );
}

void CfdMeshScreen::UpdateDisplayTab()
{
    //===== Display Tab Toggle Update =====//
    m_ShowSourcesAndWakePreview.Update( CfdMeshMgr.GetCfdSettingsPtr()->m_DrawSourceFlag.GetID() );
    m_ShowFarFieldPreview.Update( CfdMeshMgr.GetCfdSettingsPtr()->m_DrawFarPreFlag.GetID() );
    m_ShowMesh.Update( CfdMeshMgr.GetCfdSettingsPtr()->m_DrawMeshFlag.GetID() );
    m_ShowWake.Update( CfdMeshMgr.GetCfdSettingsPtr()->m_DrawWakeFlag.GetID() );
    m_ShowSymmetryPlane.Update( CfdMeshMgr.GetCfdSettingsPtr()->m_DrawSymmFlag.GetID() );
    m_ShowFarField.Update( CfdMeshMgr.GetCfdSettingsPtr()->m_DrawFarFlag.GetID() );
    m_ShowBadEdgesAndTriangles.Update( CfdMeshMgr.GetCfdSettingsPtr()->m_DrawBadFlag.GetID() );
    m_ColorTags.Update( CfdMeshMgr.GetCfdSettingsPtr()->m_ColorTagsFlag.GetID() );
}

void CfdMeshScreen::UpdateOutputTab()
{
    //===== Update File Output Strings =====//
    string stlname = CfdMeshMgr.GetCfdSettingsPtr()->GetExportFileName( vsp::CFD_STL_FILE_NAME );
    m_StlOutput.Update( truncateFileName( stlname, 40 ).c_str() );
    string polyname = CfdMeshMgr.GetCfdSettingsPtr()->GetExportFileName( vsp::CFD_POLY_FILE_NAME );
    m_PolyOutput.Update( truncateFileName( polyname, 40 ).c_str() );
    string triname = CfdMeshMgr.GetCfdSettingsPtr()->GetExportFileName( vsp::CFD_TRI_FILE_NAME );
    m_TriOutput.Update( truncateFileName( triname, 40 ).c_str() );
    string objname = CfdMeshMgr.GetCfdSettingsPtr()->GetExportFileName( vsp::CFD_OBJ_FILE_NAME );
    m_ObjOutput.Update( truncateFileName( objname, 40 ).c_str() );
    string gmshname = CfdMeshMgr.GetCfdSettingsPtr()->GetExportFileName( vsp::CFD_GMSH_FILE_NAME );
    m_MshOutput.Update( truncateFileName( gmshname, 40 ).c_str() );
    string datname = CfdMeshMgr.GetCfdSettingsPtr()->GetExportFileName( vsp::CFD_DAT_FILE_NAME );
    m_DatOutput.Update( truncateFileName( datname, 40 ).c_str() );
    string keyname = CfdMeshMgr.GetCfdSettingsPtr()->GetExportFileName( vsp::CFD_KEY_FILE_NAME );
    m_KeyOutput.Update( truncateFileName( keyname, 40 ).c_str() );
    string srfname = CfdMeshMgr.GetCfdSettingsPtr()->GetExportFileName( vsp::CFD_SRF_FILE_NAME );
    m_SrfOutput.Update( truncateFileName( srfname, 40 ).c_str() );
    string tkeyname = CfdMeshMgr.GetCfdSettingsPtr()->GetExportFileName( vsp::CFD_TKEY_FILE_NAME );
    m_TkeyOutput.Update( truncateFileName( tkeyname, 40).c_str() );

    //==== Update File Output Flags ====//
    m_StlFile.Update( CfdMeshMgr.GetCfdSettingsPtr()->GetExportFileFlag( vsp::CFD_STL_FILE_NAME )->GetID() );
    m_TaggedMultiSolid.Update( m_Vehicle->m_STLMultiSolid.GetID() );
    m_PolyFile.Update( CfdMeshMgr.GetCfdSettingsPtr()->GetExportFileFlag( vsp::CFD_POLY_FILE_NAME )->GetID() );
    m_TriFile.Update( CfdMeshMgr.GetCfdSettingsPtr()->GetExportFileFlag( vsp::CFD_TRI_FILE_NAME )->GetID() );
    m_ObjFile.Update( CfdMeshMgr.GetCfdSettingsPtr()->GetExportFileFlag( vsp::CFD_OBJ_FILE_NAME )->GetID() );
    m_MshFile.Update( CfdMeshMgr.GetCfdSettingsPtr()->GetExportFileFlag( vsp::CFD_GMSH_FILE_NAME )->GetID() );
    m_DatFile.Update( CfdMeshMgr.GetCfdSettingsPtr()->GetExportFileFlag( vsp::CFD_DAT_FILE_NAME )->GetID() );
    m_KeyFile.Update( CfdMeshMgr.GetCfdSettingsPtr()->GetExportFileFlag( vsp::CFD_KEY_FILE_NAME )->GetID() );
    m_SrfFile.Update( CfdMeshMgr.GetCfdSettingsPtr()->GetExportFileFlag( vsp::CFD_SRF_FILE_NAME )->GetID() );
    m_TkeyFile.Update( CfdMeshMgr.GetCfdSettingsPtr()->GetExportFileFlag( vsp::CFD_TKEY_FILE_NAME)->GetID() );
}

void CfdMeshScreen::UpdateSourcesTab( BaseSource* source )
{
    if ( source )
    {
        m_SourceLen1.Activate();
        m_SourceRad1.Activate();
        m_SourceName.Activate();

        m_SourceLen1.Update( source->m_Len.GetID() );
        m_SourceRad1.Update( source->m_Rad.GetID() );

        m_SourceName.Update( source->GetName().c_str() );

        if ( source->GetType() == vsp::POINT_SOURCE )
        {
            m_SourceU1.Activate();
            m_SourceW1.Activate();

            PointSource* ps = ( PointSource* )source;

            m_SourceU1.Update( ps->m_ULoc.GetID() );
            m_SourceW1.Update( ps->m_WLoc.GetID() );

            m_SourceU2.Deactivate();
            m_SourceW2.Deactivate();
            m_SourceLen2.Deactivate();
            m_SourceRad2.Deactivate();
        }
        else if ( source->GetType() == vsp::LINE_SOURCE )
        {
            m_SourceU1.Activate();
            m_SourceW1.Activate();
            m_SourceU2.Activate();
            m_SourceW2.Activate();
            m_SourceLen2.Activate();
            m_SourceRad2.Activate();

            LineSource* ps = ( LineSource* )source;

            m_SourceU1.Update( ps->m_ULoc1.GetID() );
            m_SourceW1.Update( ps->m_WLoc1.GetID() );

            m_SourceU2.Update( ps->m_ULoc2.GetID() );
            m_SourceW2.Update( ps->m_WLoc2.GetID() );

            m_SourceLen2.Update( ps->m_Len2.GetID() );
            m_SourceRad2.Update( ps->m_Rad2.GetID() );
        }
        else if ( source->GetType() == vsp::BOX_SOURCE )
        {
            m_SourceU1.Activate();
            m_SourceW1.Activate();
            m_SourceU2.Activate();
            m_SourceW2.Activate();

            BoxSource* ps = ( BoxSource* )source;

            m_SourceU1.Update( ps->m_ULoc1.GetID() );
            m_SourceW1.Update( ps->m_WLoc1.GetID() );

            m_SourceU2.Update( ps->m_ULoc2.GetID() );
            m_SourceW2.Update( ps->m_WLoc2.GetID() );

            m_SourceLen2.Deactivate();
            m_SourceRad2.Deactivate();
        }
    }
    else
    {
        m_SourceLen1.Deactivate();
        m_SourceRad1.Deactivate();
        m_SourceLen2.Deactivate();
        m_SourceRad2.Deactivate();
        m_SourceU1.Deactivate();
        m_SourceW1.Deactivate();
        m_SourceU2.Deactivate();
        m_SourceW2.Deactivate();
        m_SourceName.Deactivate();
    }

    //===== Update Sources Component Selection =====//
    m_SourcesSelectComp.UpdateItems();
}

void CfdMeshScreen::UpdateDomainTab()
{
    //===== Update Generate Buttons =====//
    m_GenerateHalfMesh.Update( CfdMeshMgr.GetCfdSettingsPtr()->m_HalfMeshFlag.GetID() );
    m_GenerateFarFieldMesh.Update( CfdMeshMgr.GetCfdSettingsPtr()->m_FarMeshFlag.GetID() );

    //===== Update Domain Sliders =====//
    m_DomainMaxEdgeLen.Update( CfdMeshMgr.GetGridDensityPtr()->m_FarMaxLen.GetID() );
    m_DomainMaxGap.Update( CfdMeshMgr.GetGridDensityPtr()->m_FarMaxGap.GetID() );
    m_DomainNumCircleSegments.Update( CfdMeshMgr.GetGridDensityPtr()->m_FarNCircSeg.GetID() );

    //===== Symmetry Plane Toggle Update =====//
    m_SymPlaneSplit.Update( CfdMeshMgr.GetCfdSettingsPtr()->m_SymSplittingOnFlag.GetID() );

    //Last parameter set to null because Abs functionality currently not available
    m_DomainLength.Update( 1, CfdMeshMgr.GetCfdSettingsPtr()->m_FarXScale.GetID(), "" );
    m_DomainWidth.Update( 1, CfdMeshMgr.GetCfdSettingsPtr()->m_FarYScale.GetID(), "" );
    m_DomainHeight.Update( 1, CfdMeshMgr.GetCfdSettingsPtr()->m_FarZScale.GetID(), "" );

    m_DomainXLoc.Update( CfdMeshMgr.GetCfdSettingsPtr()->m_FarXLocation.GetID() );
    m_DomainYLoc.Update( CfdMeshMgr.GetCfdSettingsPtr()->m_FarYLocation.GetID() );
    m_DomainZLoc.Update( CfdMeshMgr.GetCfdSettingsPtr()->m_FarZLocation.GetID() );

    //=== Domain tab GUI active areas ===//
    if ( CfdMeshMgr.GetCfdSettingsPtr()->GetFarMeshFlag() )
    {
        m_FarParametersLayout.GetGroup()->activate();

        if( CfdMeshMgr.GetCfdSettingsPtr()->GetFarCompFlag() )
        {
            m_FarBoxLayout.GetGroup()->deactivate();
            m_ComponentChoice.Activate();
        }
        else
        {
            m_FarBoxLayout.GetGroup()->activate();
            m_ComponentChoice.Deactivate();

            if( CfdMeshMgr.GetCfdSettingsPtr()->GetFarManLocFlag() )
            {
                m_FarXYZLocationLayout.GetGroup()->activate();
            }
            else
            {
                m_FarXYZLocationLayout.GetGroup()->deactivate();
            }
        }
    }
    else
    {
        m_FarParametersLayout.GetGroup()->deactivate();
    }

    //===== Update Domain Component Selection =====//
    m_ComponentChoice.UpdateItems();
}

void CfdMeshScreen::UpdateWakesTab()
{
    //===== Update Wake Sliders =====//
    m_ScaleWake.Update( CfdMeshMgr.GetCfdSettingsPtr()->m_WakeScale.GetID() );
    m_WakeAngle.Update( CfdMeshMgr.GetCfdSettingsPtr()->m_WakeAngle.GetID() );

    //===== Update Wake Component Selection =====//
    m_Comp.UpdateItems();

    //===== Update Add Wake =====//
    string wakeGeomID = CfdMeshMgr.GetWakeGeomID();
    Geom* g = m_Vehicle->FindGeom( wakeGeomID );
    if ( g )
    {
        m_AddWake.Update( g->m_WakeActiveFlag.GetID() );
    }
}

void CfdMeshScreen::LoadSetChoice()
{
    m_UseSet.ClearItems();

    vector< string > set_name_vec = m_Vehicle->GetSetNameVec();

    for ( int i = 0 ; i < ( int )set_name_vec.size() ; ++i )
    {
        m_UseSet.AddItem( set_name_vec[i].c_str() );
    }

    m_UseSet.UpdateItems();
    m_UseSet.SetVal( CfdMeshMgr.GetCfdSettingsPtr()->m_SelectedSetIndex() );
}

void CfdMeshScreen::AddOutputText( const string &text )
{
    m_ConsoleBuffer->append( text.c_str() );
    m_ConsoleDisplay->move_down();
    m_ConsoleDisplay->show_insert_position();
    Fl::flush();
}

string CfdMeshScreen::truncateFileName( const string &fn, int len )
{
    string trunc( fn );
    if ( (int)trunc.length() > len )
    {
        trunc.erase( 0, trunc.length() - len );
        trunc.replace( 0, 3, "..." );
    }
    return trunc;
}

void CfdMeshScreen::LoadDrawObjs( vector< DrawObj* > &draw_obj_vec )
{
    if ( IsShown() )
    {
        CfdMeshMgr.LoadDrawObjs( draw_obj_vec );
    }
}

void CfdMeshScreen::Show()
{
    m_ScreenMgr->SetUpdateFlag( true );
    m_FLTK_Window->show();
}

void CfdMeshScreen::Hide()
{
    m_FLTK_Window->hide();
    m_ScreenMgr->SetUpdateFlag( true );
}

void CfdMeshScreen::CallBack( Fl_Widget* w )
{
    assert( m_ScreenMgr );

    if ( w == m_SourceBrowser )
    {
        CfdMeshMgr.GUI_Val( "SourceID", m_SourceBrowser->value() - 1 );
    }

    m_ScreenMgr->SetUpdateFlag( true );
}

void CfdMeshScreen::CloseCallBack( Fl_Widget *w )
{
    Hide();
}

void CfdMeshScreen::GuiDeviceCallBack( GuiDevice* device )
{
    assert( m_ScreenMgr );

    GuiDeviceGlobalTabCallback( device );
    GuiDeviceOutputTabCallback( device );
    GuiDeviceSourcesTabCallback( device );
    GuiDeviceDomainTabCallback( device );
    GuiDeviceWakesTabCallback( device );

    if ( device == &m_MeshAndExport )
    {
        redirecter redir( std::cout, CfdMeshMgr.m_OutStream );
        CfdMeshMgr.GenerateMesh();

        // Hide all geoms.
        Vehicle* veh = m_ScreenMgr->GetVehiclePtr();
        m_Vehicle->HideAll();
    }

    m_ScreenMgr->SetUpdateFlag( true );
}

void CfdMeshScreen::GuiDeviceGlobalTabCallback( GuiDevice* device )
{
    //===== Global Source Control =====//
    if ( device == &m_GlobSrcAdjustLenLftLft )
    {
        CfdMeshMgr.AdjustAllSourceLen( 1.0 / 1.5 );
    }
    else if ( device == &m_GlobSrcAdjustLenLft )
    {
        CfdMeshMgr.AdjustAllSourceLen( 1.0 / 1.1 );
    }
    else if ( device == &m_GlobSrcAdjustLenRht )
    {
        CfdMeshMgr.AdjustAllSourceLen( 1.1 );
    }
    else if ( device == &m_GlobSrcAdjustLenRhtRht )
    {
        CfdMeshMgr.AdjustAllSourceLen( 1.5 );
    }
    else if ( device == &m_GlobSrcAdjustRadLftLft )
    {
        CfdMeshMgr.AdjustAllSourceRad( 1.0 / 1.5 );
    }
    else if ( device == &m_GlobSrcAdjustRadLft )
    {
        CfdMeshMgr.AdjustAllSourceRad( 1.0 / 1.1 );
    }
    else if ( device == &m_GlobSrcAdjustRadRht )
    {
        CfdMeshMgr.AdjustAllSourceRad( 1.1 );
    }
    else if ( device == &m_GlobSrcAdjustRadRhtRht )
    {
        CfdMeshMgr.AdjustAllSourceRad( 1.5 );
    }
    //Use Set
    else if ( device == &m_UseSet )
    {
        CfdMeshMgr.GetCfdSettingsPtr()->m_SelectedSetIndex = m_UseSet.GetVal();
    }
}

void CfdMeshScreen::GuiDeviceOutputTabCallback( GuiDevice* device )
{
    if ( device == &m_SelectStlFile )
    {
        string newfile = m_ScreenMgr->GetSelectFileScreen()->FileChooser( "Select .stl file.", "*.stl" );
        if ( newfile.compare( "" ) != 0 )
        {
            CfdMeshMgr.GetCfdSettingsPtr()->SetExportFileName( newfile, vsp::CFD_STL_FILE_NAME );
        }
    }
    else if ( device == &m_SelectPolyFile )
    {
        string newfile = m_ScreenMgr->GetSelectFileScreen()->FileChooser( "Select .poly file.", "*.poly" );
        if ( newfile.compare( "" ) != 0 )
        {
            CfdMeshMgr.GetCfdSettingsPtr()->SetExportFileName( newfile, vsp::CFD_POLY_FILE_NAME );
        }
    }
    else if ( device == &m_SelectTriFile )
    {
        string newfile = m_ScreenMgr->GetSelectFileScreen()->FileChooser( "Select .tri file.", "*.tri" );
        if ( newfile.compare( "" ) != 0 )
        {
            CfdMeshMgr.GetCfdSettingsPtr()->SetExportFileName( newfile, vsp::CFD_TRI_FILE_NAME );
        }
    }
    else if ( device == &m_SelectObjFile  )
    {
        string newfile = m_ScreenMgr->GetSelectFileScreen()->FileChooser( "Select .obj file.", "*.obj" );
        if ( newfile.compare( "" ) != 0 )
        {
            CfdMeshMgr.GetCfdSettingsPtr()->SetExportFileName( newfile, vsp::CFD_OBJ_FILE_NAME );
        }
    }
    else if ( device == &m_SelectMshFile )
    {
        string newfile = m_ScreenMgr->GetSelectFileScreen()->FileChooser( "Select .msh file.", "*.msh" );
        if ( newfile.compare( "" ) != 0 )
        {
            CfdMeshMgr.GetCfdSettingsPtr()->SetExportFileName( newfile, vsp::CFD_GMSH_FILE_NAME );
        }
    }
    else if ( device == &m_SelectDatFile )
    {
        string newfile = m_ScreenMgr->GetSelectFileScreen()->FileChooser( "Select NASCART .dat file.", "*.dat" );
        if ( newfile.compare( "" ) != 0 )
        {
            CfdMeshMgr.GetCfdSettingsPtr()->SetExportFileName( newfile, vsp::CFD_DAT_FILE_NAME );
        }
    }
    else if ( device == &m_SelectKeyFile )
    {
        string newfile = m_ScreenMgr->GetSelectFileScreen()->FileChooser( "Select NASCART .key file.", "*.key" );
        if ( newfile.compare( "" ) != 0 )
        {
            CfdMeshMgr.GetCfdSettingsPtr()->SetExportFileName( newfile, vsp::CFD_KEY_FILE_NAME );
        }
    }
    else if ( device == &m_SelectSrfFile )
    {
        string newfile = m_ScreenMgr->GetSelectFileScreen()->FileChooser( "Select .srf file.", "*.srf" );
        if ( newfile.compare( "" ) != 0 )
        {
            CfdMeshMgr.GetCfdSettingsPtr()->SetExportFileName( newfile, vsp::CFD_SRF_FILE_NAME );
        }
    }
    else if ( device == &m_SelectTkeyFile )
    {
        string newfile = m_ScreenMgr->GetSelectFileScreen()->FileChooser( "Select .tkey file.", "*.tkey" );
        if ( newfile.compare( "" ) != 0 )
        {
            CfdMeshMgr.GetCfdSettingsPtr()->SetExportFileName( newfile, vsp::CFD_TKEY_FILE_NAME );
        }
    }
}

void CfdMeshScreen::GuiDeviceSourcesTabCallback( GuiDevice* device )
{
    if ( device == &m_AddDefaultSources )
    {
        CfdMeshMgr.AddDefaultSourcesCurrGeom();
    }
    else if ( device == &m_AddSource )
    {
        int type = m_SourcesType.GetVal();
        if ( type >= 0 && type < vsp::NUM_SOURCE_TYPES )
        {
            CfdMeshMgr.AddSource( type );
        }
    }
    else if ( device == &m_DeleteSource )
    {
        CfdMeshMgr.DeleteCurrSource();
    }
    else if ( device == &m_SourcesSelectComp )
    {
        //==== Load List of Parts for Comp ====//
        int id = m_SourcesSelectComp.GetVal();
        CfdMeshMgr.SetCurrSourceGeomID( m_GeomVec[ id ] );
        CfdMeshMgr.SetCurrMainSurfIndx( 0 );
    }
    else if ( device == &m_SourcesSelectSurface )
    {
        int id = m_SourcesSelectSurface.GetVal();
        CfdMeshMgr.SetCurrMainSurfIndx( id );
    }
    else if ( device == &m_SourceName )
    {
        //===== Update Sources =====//
        BaseSource* source = CfdMeshMgr.GetCurrSource();
        source->SetName( m_SourceName.GetString() );
    }
}

void CfdMeshScreen::GuiDeviceDomainTabCallback( GuiDevice* device )
{
    if ( device == &m_FarFieldTypeComponent )
    {
        CfdMeshMgr.GetCfdSettingsPtr()->SetFarCompFlag( true );

        if ( m_FarFieldTypeBox.GetValue() )
        {
            m_FarFieldTypeComponent.GetFlButton()->value( -1 );
            m_FarFieldTypeBox.GetFlButton()->value( 0 );
        }
        else
        {
            m_FarFieldTypeComponent.GetFlButton()->value( -1 );
        }
    }
    else if ( device == &m_FarFieldTypeBox )
    {
        CfdMeshMgr.GetCfdSettingsPtr()->SetFarCompFlag( false );

        if ( m_FarFieldTypeComponent.GetValue() )
        {
            m_FarFieldTypeBox.GetFlButton()->value( -1 );
            m_FarFieldTypeComponent.GetFlButton()->value( 0 );
        }
        else
        {
            m_FarFieldTypeBox.GetFlButton()->value( -1 );
        }
    }
    else if ( device == &m_DomainRel )
    {
        CfdMeshMgr.GetCfdSettingsPtr()->SetFarAbsSizeFlag( false );

        if ( (int)m_DomainAbs.GetFlButton()->value() )
        {
            m_DomainRel.GetFlButton()->value( 1 );
            m_DomainAbs.GetFlButton()->value( 0 );
        }
        else
        {
            m_DomainRel.GetFlButton()->value( 1 );
        }
    }
    else if ( device == &m_DomainAbs )
    {
        CfdMeshMgr.GetCfdSettingsPtr()->SetFarAbsSizeFlag( true );

        if ( (int)m_DomainRel.GetFlButton()->value() )
        {
            m_DomainAbs.GetFlButton()->value( 1 );
            m_DomainRel.GetFlButton()->value( 0 );
        }
        else
        {
            m_DomainAbs.GetFlButton()->value( 1 );
        }
    }
    else if ( device == &m_DomainCen )
    {
        CfdMeshMgr.GetCfdSettingsPtr()->SetFarManLocFlag( false );

        if ( (int)m_DomainMan.GetFlButton()->value() )
        {
            m_DomainCen.GetFlButton()->value( 1 );
            m_DomainMan.GetFlButton()->value( 0 );
        }
        else
        {
            m_DomainCen.GetFlButton()->value( 1 );
        }
    }
    else if ( device == &m_DomainMan )
    {
        CfdMeshMgr.GetCfdSettingsPtr()->SetFarManLocFlag( true );

        if ( (int)m_DomainCen.GetFlButton()->value() )
        {
            m_DomainMan.GetFlButton()->value( 1 );
            m_DomainCen.GetFlButton()->value( 0 );
        }
        else
        {
            m_DomainMan.GetFlButton()->value( 1 );
        }
    }
}

void CfdMeshScreen::GuiDeviceWakesTabCallback( GuiDevice* device )
{
    if ( device == &m_Comp )
    {
        CfdMeshMgr.SetWakeGeomID( m_WingGeomVec[ m_Comp.GetVal() ] );
    }
}