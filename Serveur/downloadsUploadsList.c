//=============================================================================
// Projet : Winrell (server)
// Fichier : downloadsUploadsList.c
//
//=============================================================================

#include "server.h"


//=============================================================================
//                                Structure
//=============================================================================

struct {
    TCHAR* pszText;
    int cx;
} columnMap[] = {
    { "File Name", 200 },
    { "Size",       70 },
    { "State",      80 },
    { "Progress",  120 },
    { "Client",    100 },
    { "Details",   200 },
    { "Start At",   80 },
    { "End At",     80 },
    { 0 }
};

//=============================================================================
//                       DownloadsUploadsListProc
//
//=============================================================================

LRESULT CALLBACK DownloadsUploadsListProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg)
    {
        case WM_CREATE:
            // Création de la listView
            hwDownloadsUploadsListView = CreateWindow(WC_LISTVIEW, NULL,
                                         LVS_REPORT | WS_TABSTOP | WS_BORDER | WS_CHILD | WS_VISIBLE,
                                         0, 0, 0, 0, hwnd, (HMENU) LST_DOWNLOADS_UPLOADS, hInst, NULL);
            // Création de l'image liste
            HICON hIcon;
            HIMAGELIST hImgList = ImageList_Create(GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), ILC_COLOR32 , 2, 2); // 2 == max icones
            ImageList_SetBkColor(hImgList, GetSysColor(COLOR_WINDOW));
            hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_DOWNLOAD));
            ImageList_AddIcon(hImgList, hIcon);
            hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_UPLOAD));
            ImageList_AddIcon(hImgList, hIcon);
            // On lui associe l'image liste
            ListView_SetImageList(hwDownloadsUploadsListView, hImgList, LVSIL_SMALL);
            // Ajout des colonnes à la ListView
            BuildListViewColumns(hwDownloadsUploadsListView);
            return 0;
        case WM_CLOSE:
            /* On cahe la fenêtre */
            ShowWindow(hwFenDownloadsUploads, SW_HIDE);
            //PostQuitMessage(0);
            return 0;
        case WM_NOTIFY:
        {
            NMHDR* pHdr = (NMHDR*) lParam;
            if(pHdr->idFrom == LST_DOWNLOADS_UPLOADS  &&  pHdr->code == NM_CUSTOMDRAW)
                return HandleCustomDraw(hwDownloadsUploadsListView, (NMLVCUSTOMDRAW*) pHdr);
            break;
        }
        case WM_SIZE:
            if(wParam != SIZE_MAXHIDE && wParam != SIZE_MAXSHOW  && wParam != SIZE_MINIMIZED)
                SetWindowPos(hwDownloadsUploadsListView, NULL, 10, 10, LOWORD(lParam) - 20, HIWORD(lParam) - 20, SWP_NOZORDER);
            break;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

//=============================================================================
//                           HandleCustomDraw
//
//=============================================================================

LRESULT HandleCustomDraw(HWND hwndListView, NMLVCUSTOMDRAW* pcd)
{
	TCHAR buffer[16];
	LVITEM item;

    switch(pcd->nmcd.dwDrawStage) {
        case CDDS_PREPAINT:
            /* Tell the control we are interested in per-item notifications.
             * (We need it just to tell the control we want per-subitem
             * notifications.) */
            return CDRF_DODEFAULT | CDRF_NOTIFYITEMDRAW;

        case (CDDS_ITEM | CDDS_PREPAINT):
            /* Tell the control we are interested in per-subitem notifications. */
            return CDRF_DODEFAULT | CDRF_NOTIFYSUBITEMDRAW;

        case (CDDS_ITEM | CDDS_SUBITEM | CDDS_PREPAINT):
            switch(pcd->iSubItem) {
                case 2:
                    /* Customize "State" column by marking some state levels
                     * with appropriate color. */
                    item.iSubItem = pcd->iSubItem;
                    item.pszText = buffer;
                    item.cchTextMax = sizeof(buffer) / sizeof(buffer[0]);
                    SendMessage(hwndListView, LVM_GETITEMTEXT, pcd->nmcd.dwItemSpec, (LPARAM) &item);
                    if(lstrcmp("Error", buffer) == 0)
                        pcd->clrText = RGB(190,0,0);
					else if(lstrcmp("Done", buffer) == 0)
						pcd->clrText = RGB(0,160,0);
                    /* Let the control do the painting itself with the new color. */
                    return CDRF_DODEFAULT;

                case 3:
                {
                    /* Customize "Progress" column. We paint simple progress
                     * indicator. */
                    int iProgress = pcd->nmcd.lItemlParam;
                    int cx;
                    HDC hdc = pcd->nmcd.hdc;
					COLORREF clrBack;
                    HBRUSH hBackBrush;
                    HBRUSH hProgressBrush;
                    HBRUSH hOldBrush;
					HPEN hPen;
					HPEN hOldPen;
                    RECT rc;

					clrBack = pcd->clrTextBk;
					if(clrBack == CLR_NONE || clrBack == CLR_DEFAULT)
						clrBack = RGB(255,255,255);

                    hBackBrush = CreateSolidBrush(clrBack);
                    hProgressBrush = CreateSolidBrush(RGB(190,190,255));
					hPen = CreatePen(PS_SOLID, 0, RGB(190,190,255));

                    hOldBrush = (HBRUSH) SelectObject(hdc, hBackBrush);
                    FillRect(hdc, &pcd->nmcd.rc, hBackBrush);

                    cx = pcd->nmcd.rc.right - pcd->nmcd.rc.left - 6;
                    if(cx < 0)
                        cx = 0;
                    rc.left = pcd->nmcd.rc.left + 3;
                    rc.top = pcd->nmcd.rc.top + 2;
                    rc.right = rc.left + cx * iProgress / 100;
                    rc.bottom = pcd->nmcd.rc.bottom - 2;
                    SelectObject(hdc, hProgressBrush);
                    FillRect(hdc, &rc, hProgressBrush);

					rc.right = pcd->nmcd.rc.right - 3;
					SelectObject(hdc, GetStockObject(HOLLOW_BRUSH));
					hOldPen = SelectObject(hdc, hPen);
					Rectangle(hdc, rc.left, rc.top, rc.right, rc.bottom);

                    item.iSubItem = pcd->iSubItem;
                    item.pszText = buffer;
                    item.cchTextMax = sizeof(buffer) / sizeof(buffer[0]);
                    SendMessage(hwndListView, LVM_GETITEMTEXT, pcd->nmcd.dwItemSpec, (LPARAM) &item);
					DrawText(hdc, buffer, -1, &rc, 
							DT_CENTER | DT_VCENTER | DT_NOPREFIX | DT_SINGLELINE | DT_END_ELLIPSIS);

                    SelectObject(hdc, hOldBrush);
                    DeleteObject(hProgressBrush);
                    DeleteObject(hBackBrush);
					SelectObject(hdc, hOldPen);
					DeleteObject(hPen);

                    /* Tell the control to not paint as we did so. */
                    return CDRF_SKIPDEFAULT;
                }
                default:
                    pcd->clrText = RGB(0,0,0);
            }
            break;
    }

    /* For all unhandled cases, we let the control do the default. */
    return CDRF_DODEFAULT;
}

//=============================================================================
//                            BuildListViewColumns
//
//=============================================================================

void BuildListViewColumns(HWND hwndListView)
{
    LVCOLUMN col;
    LVITEM item;
    TCHAR pszBuffer[16];
    int i;

    col.mask = LVCF_WIDTH | LVCF_TEXT;
    for(i = 0; columnMap[i].pszText != NULL; i++)
    {
        col.pszText = columnMap[i].pszText;
        col.cx = columnMap[i].cx;
        SendMessage(hwndListView, LVM_INSERTCOLUMN, i, (LPARAM) &col);
    }
}

//=============================================================================
//                      AddToDownloadsUploadsListView
//
//=============================================================================

int AddToDownloadsUploadsListView(HWND hwndListView, UploadDownload infos)
{
    LVITEM item;
    TCHAR pszBuffer[16];
    int pos = ListView_GetItemCount(hwndListView);

    item.mask = LVIF_PARAM | LVIF_TEXT | LVIF_IMAGE;
    item.iItem = pos;
    item.iSubItem = 0;
    if (infos.isDownload)
       item.iImage = 0;
    else
       item.iImage = 1;
    item.pszText = infos.fileName;
    item.lParam = infos.iProgress;
    SendMessage(hwndListView, LVM_INSERTITEM, 0, (LPARAM) &item);

    item.mask = LVIF_TEXT;
    
    item.iSubItem = 1;
    item.pszText = infos.fileSize;
    SendMessage(hwndListView, LVM_SETITEM, 0, (LPARAM) &item);
    
    item.iSubItem = 2;
    item.pszText = infos.state;
    SendMessage(hwndListView, LVM_SETITEM, 0, (LPARAM) &item);

    item.iSubItem = 3;
    item.pszText = pszBuffer;
    sprintf(pszBuffer, "%d %%", infos.iProgress);
    SendMessage(hwndListView, LVM_SETITEM, 0, (LPARAM) &item);
    
    item.iSubItem = 4;
    item.pszText = infos.client;
    SendMessage(hwndListView, LVM_SETITEM, 0, (LPARAM) &item);
    
    item.iSubItem = 5;
    item.pszText = infos.details;
    SendMessage(hwndListView, LVM_SETITEM, 0, (LPARAM) &item);
    
    item.iSubItem = 6;
    item.pszText = infos.startAt;
    SendMessage(hwndListView, LVM_SETITEM, 0, (LPARAM) &item);
    
    item.iSubItem = 7;
    item.pszText = infos.endAt;
    SendMessage(hwndListView, LVM_SETITEM, 0, (LPARAM) &item);
    
    // scrolling
    ListView_EnsureVisible(hwndListView, pos, FALSE);
    
    return pos;
}

//=============================================================================
//                      UpdateDownloadUploadProgress
//
//=============================================================================

void UpdateDownloadUploadProgress(HWND hwndListView, UploadDownload infos, int itemPos, BOOL updateState, BOOL updateDetails)
{
    LVITEM item;
    TCHAR pszBuffer[16];
    
    item.mask = LVIF_PARAM;
    item.iItem = itemPos;
    item.iSubItem = 0;
    item.lParam = infos.iProgress;
    SendMessage(hwndListView, LVM_SETITEM, itemPos, (LPARAM) &item);

    item.mask = LVIF_TEXT; // change mask
    
    /*
    item.iSubItem = 1;
    item.pszText = infos.fileSize;
    SendMessage(hwndListView, LVM_SETITEMTEXT, itemPos, (LPARAM) &item);
    */
    
    if (updateState)
    {
        item.iSubItem = 2;
        item.pszText = infos.state;
        SendMessage(hwndListView, LVM_SETITEMTEXT, itemPos, (LPARAM) &item);
    }

    item.iSubItem = 3;
    item.pszText = pszBuffer;
    sprintf(pszBuffer, "%d %%", infos.iProgress);
    SendMessage(hwndListView, LVM_SETITEMTEXT, itemPos, (LPARAM) &item);
    
    /*
    item.iSubItem = 4;
    item.pszText = infos.client;
    SendMessage(hwndListView, LVM_SETITEMTEXT, itemPos, (LPARAM) &item);
    */
    
    if (updateDetails)
    {
        item.iSubItem = 5;
        item.pszText = infos.details;
        SendMessage(hwndListView, LVM_SETITEMTEXT, itemPos, (LPARAM) &item);
        
        item.iSubItem = 7;
        item.pszText = infos.endAt;
        SendMessage(hwndListView, LVM_SETITEMTEXT, itemPos, (LPARAM) &item);
    }
    
    /*
    item.iSubItem = 6;
    item.pszText = infos.startAt;
    SendMessage(hwndListView, LVM_SETITEMTEXT, itemPos, (LPARAM) &item);
    */
}
