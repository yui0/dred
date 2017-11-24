// Copyright (C) 2017 David Reid. See included LICENSE file.

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//
// WIN32
//
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#ifdef DTK_WIN32
dtk_dialog_result dtk_convert_dialog_result_from_win32(int dialogResultWin32)
{
    switch (dialogResultWin32)
    {
        case IDOK:      return DTK_DIALOG_RESULT_OK;
        case IDCANCEL:  return DTK_DIALOG_RESULT_CANCEL;
        case IDYES:     return DTK_DIALOG_RESULT_YES;
        case IDNO:      return DTK_DIALOG_RESULT_NO;

        case IDABORT:
        case IDRETRY:
        case IDIGNORE:
        case IDTRYAGAIN:
        case IDCONTINUE:
        default: break;
    }

    return 0;
}

UINT dtk_convert_dialog_buttons_to_win32(dtk_dialog_buttons buttons)
{
    switch (buttons)
    {
        case DTK_DIALOG_BUTTONS_OK:          return MB_OK;
        case DTK_DIALOG_BUTTONS_OKCANCEL:    return MB_OKCANCEL;
        case DTK_DIALOG_BUTTONS_YESNO:       return MB_YESNO;
        case DTK_DIALOG_BUTTONS_YESNOCANCEL: return MB_YESNOCANCEL;
        default: break;
    }

    return 0;
}

dtk_dialog_result dtk_message_box__win32(dtk_window* pParentWindow, const char* text, const char* title, dtk_dialog_buttons buttons)
{
    return dtk_convert_dialog_result_from_win32(MessageBoxA((pParentWindow == NULL) ? NULL : (HWND)pParentWindow->win32.hWnd, text, title, dtk_convert_dialog_buttons_to_win32(buttons)));
}

dtk_dialog_result dtk_show_color_picker_dialog__win32(dtk_context* pTK, dtk_window* pOwnerWindow, dtk_color initialColor, dtk_color* pColorOut)
{
    (void)pTK;
    dtk_assert(pTK != NULL);

    static COLORREF prevcolors[16] = {
        RGB(255, 255, 255), RGB(255, 255, 255), RGB(255, 255, 255), RGB(255, 255, 255),
        RGB(255, 255, 255), RGB(255, 255, 255), RGB(255, 255, 255), RGB(255, 255, 255),
        RGB(255, 255, 255), RGB(255, 255, 255), RGB(255, 255, 255), RGB(255, 255, 255),
        RGB(255, 255, 255), RGB(255, 255, 255), RGB(255, 255, 255), RGB(255, 255, 255),
    };

    CHOOSECOLORA cc;
    ZeroMemory(&cc, sizeof(cc));
    cc.lStructSize = sizeof(cc);
    cc.hwndOwner = (pOwnerWindow == NULL) ? NULL : (HWND)pOwnerWindow->win32.hWnd;
    cc.rgbResult = RGB(initialColor.r, initialColor.g, initialColor.b);
    cc.lpCustColors = prevcolors;
    cc.Flags = CC_RGBINIT | CC_ANYCOLOR | CC_FULLOPEN;

    if (!ChooseColorA(&cc)) {
        return DTK_DIALOG_RESULT_CANCEL;
    }

    pColorOut->r = GetRValue(cc.rgbResult);
    pColorOut->g = GetGValue(cc.rgbResult);
    pColorOut->b = GetBValue(cc.rgbResult);
    pColorOut->a = 255;

    return DTK_DIALOG_RESULT_OK;
}

dtk_dialog_result dtk_show_font_picker_dialog__win32(dtk_context* pTK, dtk_window* pOwnerWindow, const dtk_font_desc* pDefaultFontDesc, dtk_font_desc* pDescOut)
{
    (void)pTK;
    dtk_assert(pTK != NULL);

    LOGFONTA lf;
    ZeroMemory(&lf, sizeof(lf));
    lf.lfHeight = -13;
    lf.lfWeight = FW_REGULAR;
    lf.lfItalic = FALSE;

    if (pDefaultFontDesc != NULL) {
        dtk_strncpy_s(lf.lfFaceName, sizeof(lf.lfFaceName), pDefaultFontDesc->family, _TRUNCATE);
        lf.lfHeight = -(LONG)pDefaultFontDesc->size;

        switch (pDefaultFontDesc->weight)
        {
        case dtk_font_weight_medium:      lf.lfWeight = FW_MEDIUM;     break;
        case dtk_font_weight_thin:        lf.lfWeight = FW_THIN;       break;
        case dtk_font_weight_extra_light: lf.lfWeight = FW_EXTRALIGHT; break;
        case dtk_font_weight_light:       lf.lfWeight = FW_LIGHT;      break;
        case dtk_font_weight_semi_bold:   lf.lfWeight = FW_SEMIBOLD;   break;
        case dtk_font_weight_bold:        lf.lfWeight = FW_BOLD;       break;
        case dtk_font_weight_extra_bold:  lf.lfWeight = FW_EXTRABOLD;  break;
        case dtk_font_weight_heavy:       lf.lfWeight = FW_HEAVY;      break;
        default: break;
        }

        if (pDefaultFontDesc->slant == dtk_font_slant_italic || pDefaultFontDesc->slant == dtk_font_slant_oblique) {
            lf.lfItalic = TRUE;
        }
    }

    CHOOSEFONTA cf;
    ZeroMemory(&cf, sizeof(cf));
    cf.lStructSize = sizeof(cf);
    cf.hwndOwner = (pOwnerWindow == NULL) ? NULL : (HWND)pOwnerWindow->win32.hWnd;
    cf.hDC = GetDC((pOwnerWindow == NULL) ? NULL : (HWND)pOwnerWindow->win32.hWnd);
    cf.lpLogFont = &lf;
    cf.Flags = CF_INITTOLOGFONTSTRUCT | CF_BOTH;

    if (!ChooseFontA(&cf)) {
        return DTK_DIALOG_RESULT_CANCEL;
    }

    dtk_strcpy_s(pDescOut->family, sizeof(pDescOut->family), lf.lfFaceName);

    if (lf.lfHeight < 0) {
        pDescOut->size = (float)-lf.lfHeight;
    } else {
        pDescOut->size = (float)lf.lfHeight;
    }

    pDescOut->weight = dtk_font_weight_default;
    switch (lf.lfWeight)
    {
    case FW_MEDIUM:     pDescOut->weight = dtk_font_weight_medium;      break;
    case FW_THIN:       pDescOut->weight = dtk_font_weight_thin;        break;
    case FW_EXTRALIGHT: pDescOut->weight = dtk_font_weight_extra_light; break;
    case FW_LIGHT:      pDescOut->weight = dtk_font_weight_light;       break;
    case FW_SEMIBOLD:   pDescOut->weight = dtk_font_weight_semi_bold;   break;
    case FW_BOLD:       pDescOut->weight = dtk_font_weight_bold;        break;
    case FW_EXTRABOLD:  pDescOut->weight = dtk_font_weight_extra_bold;  break;
    case FW_HEAVY:      pDescOut->weight = dtk_font_weight_heavy;       break;
    default: break;
    }

    pDescOut->slant = dtk_font_slant_none;
    if (lf.lfItalic) {
        pDescOut->slant = dtk_font_slant_italic;
    }

    return DTK_DIALOG_RESULT_OK;
}


// Converts the given extensions filter string to the format required by OPENFILENAME and SAVEFILENAME
//
// Example: Input: "jpg,jpeg,png"; Output: "*.jpg;*.jpeg;*.png"
//
// Free the returned string with dtk_free(). If NULL is returned, it means out of memory.
char* dtk_convert_file_dialog_extensions_filter_string_to_win32(const char* pExtensions, size_t* pLengthOut)
{
    dtk_assert(pExtensions != NULL);
    if (pExtensions[0] == '\0') pExtensions = "*";

    const char* pNextExtension = pExtensions;

    // The first step is to calculate the required size of the buffer.
    size_t bufferSize = 0;
    while (pNextExtension[0] != '\0') { // For each extension...
        size_t i = 0;
        for (;;) {  // For each character in the extension...
            if (pNextExtension[i] == ',' || pNextExtension[i] == '\0') {
                bufferSize += i + 3;    // +3 for '*', '.' and ';'

                pNextExtension += i;
                if (pNextExtension[0] == ',') {
                    pNextExtension += 1;    // <-- Skip past the ','.
                }
                pNextExtension = dtk_ltrim(pNextExtension); // <-- Skip past any whitespace between the extensions.

                break;
            } else {
                i += 1;
            }
        }
    }

    char* pExtensionsWin32 = (char*)dtk_malloc(bufferSize + 1);    // +1 for null terminator.
    if (pExtensionsWin32 == NULL) {
        return NULL;
    }

    
    size_t len = 0;
    pNextExtension = pExtensions;
    while (pNextExtension[0] != '\0') { // For each extension...
        // Prefix.
        size_t prefixLen = 0;
        if (len == 0) {
            prefixLen = 2;  // No need for the ';' separator.
        } else {
            prefixLen = 3;
            pExtensionsWin32[len + 0] = ';';
            len += 1;
        }

        pExtensionsWin32[len + 0] = '*';
        pExtensionsWin32[len + 1] = '.';
        len += 2;

        for (;;) {  // For each character in the extension...
            if (pNextExtension[0] == '\0') {
                break;
            }

            if (pNextExtension[0] == ',') {
                pNextExtension += 1;    // <-- Skip past the ','.
                pNextExtension = dtk_ltrim(pNextExtension); // <-- Skip past any whitespace between the extensions.
                break;
            }

            pExtensionsWin32[len] = pNextExtension[0];

            pNextExtension += 1;
            len += 1;
        }
    }

    // Null terminate.
    pExtensionsWin32[len] = '\0';

    if (pLengthOut) *pLengthOut = len;
    return pExtensionsWin32;
}

// Converts the given DTK-style extension filters to the format required by OPENFILENAME and SAVEFILENAME.
//
// Example:
//     Input:  {"All, "", "Text Files", "txt,csv", NULL}
//     Output: "All (*.*)\0*.*\0Text Files (*.txt,*.csv)\0*.txt;*.csv\0"
//
// Free the returned string with dtk_free(). If NULL is returned, it means out of memory.
char* dtk_convert_file_dialog_filters_to_win32(const char** ppExtensionFilters)
{
    // This is inefficient due to the realloc()'s, but it's not too much of a big deal because it'll be drowned out by the amount of
    // time it takes for Windows to actually show the dialog. This can be improved later if need be - happy for contributions!
    char* pFilter = NULL;
    size_t filterLen = 0;

    const char** ppNextFilter = ppExtensionFilters;
    for (;;) {
        const char* pName = ppNextFilter[0];
        if (pName == NULL) {
            break;
        }

        const char* pExtensions = ppNextFilter[1];
        if (pExtensions == NULL) {
            break;
        }

        size_t extensionsFilterWin32Len;
        char* pExtensionsFilterWin32 = dtk_convert_file_dialog_extensions_filter_string_to_win32(pExtensions, &extensionsFilterWin32Len);
        if (pExtensionsFilterWin32 == NULL) {
            dtk_free(pExtensionsFilterWin32);
            return NULL;
        }

        size_t nameLen = strlen(pName);

        //                           space and (                  ) and terminator                 terminator
        size_t thisFilterLen = nameLen + 2 + extensionsFilterWin32Len + 2 + extensionsFilterWin32Len + 1;
        char* pNewFilter = (char*)dtk_realloc(pFilter, filterLen + thisFilterLen);
        if (pNewFilter == NULL) {
            dtk_free(pFilter);
            dtk_free(pExtensionsFilterWin32);
            return NULL;
        }
        pFilter = pNewFilter;

        dtk_strcpy_s(pFilter + filterLen, thisFilterLen, pName);
        pFilter[filterLen + nameLen + 0] = ' ';
        pFilter[filterLen + nameLen + 1] = '(';
        dtk_strcpy_s(pFilter + filterLen + nameLen + 2, thisFilterLen - nameLen - 2, pExtensionsFilterWin32);
        pFilter[filterLen + nameLen + 2 + extensionsFilterWin32Len + 0] = ')';
        pFilter[filterLen + nameLen + 2 + extensionsFilterWin32Len + 1] = '\0';
        dtk_strcpy_s(pFilter + filterLen + nameLen + 2 + extensionsFilterWin32Len + 2, extensionsFilterWin32Len + 1, pExtensionsFilterWin32);
        filterLen += thisFilterLen;

        ppNextFilter += 2;
        dtk_free(pExtensionsFilterWin32);
    }

    // Final null terminator.
    char* pNewFilter = (char*)dtk_realloc(pFilter, filterLen + 1);
    if (pNewFilter == NULL) {
        dtk_free(pFilter);
        return NULL;
    }
    pFilter = pNewFilter;
    pFilter[filterLen] = '\0';

    return pFilter;
}

dtk_dialog_result dtk_show_open_file_dialog__win32(dtk_window* pParentWindow, dtk_open_file_dialog_options* pOptions, char*** pppSelectedFilePaths)
{
    dtk_assert(pOptions != NULL);

    // Example filter for OPENFILENAME: "All (*.*)\0*.*\0Text Files (*.txt,*.csv)\0*.txt;*.csv\0";

    // Limit to 32K for the moment. Too big for the stack? Consider allocating something larger on the heap if this becomes an issue.
    char filePaths[32768];
    if (pOptions->pDefaultPath != NULL) {
        dtk_strcpy_s(filePaths, sizeof(filePaths), pOptions->pDefaultPath);
    } else {
        filePaths[0] = '\0';
    }
    
    // Filter.
    char* pFilter = dtk_convert_file_dialog_filters_to_win32(pOptions->ppExtensionFilters);
    if (pFilter == NULL) {
        return DTK_OUT_OF_MEMORY;
    }

    OPENFILENAMEA ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = (pParentWindow == NULL) ? NULL : (HWND)pParentWindow->win32.hWnd;
    ofn.lpstrFile = filePaths;
    ofn.nMaxFile = sizeof(filePaths);
    ofn.lpstrFilter = pFilter;
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_EXPLORER;
    if (pOptions->fileMustExist) {
        ofn.Flags |= OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
    }
    if (pOptions->multiSelect) {
        ofn.Flags |= OFN_ALLOWMULTISELECT;
    }

    if (!GetOpenFileNameA(&ofn)) {
        dtk_free(pFilter);
        return DTK_DIALOG_RESULT_CANCEL;
    }


    // Output.
    if (dtk_is_directory(filePaths)) {
        // Multiple files. This is done in two passes. The first pass calculates the size of the output buffer. The second pass fills it in.
        dtk_uint32 fileCount = 0;
        const char* directory = filePaths;  // For clarity.
        size_t directoryLen = strlen(filePaths);
        

        size_t outputBufferSize = sizeof(char*);    // Initial value for the null terminating item.
        const char* pNextFileName = filePaths + strlen(filePaths) + 1;  // +1 for null terminator.
        while (pNextFileName[0] != '\0') {
            size_t nextFilePathLen = directoryLen + 1 + strlen(pNextFileName);  // +1 for the slash between the directory and the file name.
            outputBufferSize += sizeof(char*) + nextFilePathLen + 1;  // +1 for null terminator.
            pNextFileName += nextFilePathLen + 1;
            fileCount += 1;
        }

        char* pOutputBuffer = (char*)dtk_malloc(outputBufferSize);
        if (pOutputBuffer == NULL) {
            dtk_free(pFilter);
            return DTK_OUT_OF_MEMORY;
        }

        char** ppItemPointers = (char**)pOutputBuffer;

        size_t runningOffset = sizeof(char*) * (fileCount+1);    // +1 for the null-terminating item.

        dtk_uint32 iFile = 0;
        pNextFileName = filePaths + strlen(filePaths) + 1;  // +1 for null terminator.
        while (pNextFileName[0] != '\0') {
            size_t nextFileNameLen = strlen(pNextFileName);
            size_t nextFilePathLen = directoryLen + 1 + nextFileNameLen; // +1 for the slash between the directory and the file name.
            
            ppItemPointers[iFile] = (pOutputBuffer + runningOffset);
            dtk_strcpy_s(ppItemPointers[iFile], nextFilePathLen+1, directory);
            dtk_strcat_s(ppItemPointers[iFile], nextFilePathLen+1, "\\");
            dtk_strcat_s(ppItemPointers[iFile], nextFilePathLen+1, pNextFileName);

            pNextFileName += nextFileNameLen + 1;
            runningOffset += nextFilePathLen + 1;
            iFile += 1;
        }

        ppItemPointers[iFile] = NULL;

        if (pppSelectedFilePaths) *pppSelectedFilePaths = ppItemPointers;
    } else {
        // Single file.
        size_t outputBufferSize = sizeof(char*)*2 + strlen(filePaths) + 1;
        char* pOutputBuffer = (char*)dtk_malloc(outputBufferSize);
        if (pOutputBuffer == NULL) {
            dtk_free(pFilter);
            return DTK_OUT_OF_MEMORY;
        }

        char** ppItemPointers = (char**)pOutputBuffer;
        ppItemPointers[0] = pOutputBuffer + (sizeof(char*)*2);
        ppItemPointers[1] = NULL;

        dtk_strcpy(ppItemPointers[0], filePaths);

        if (pppSelectedFilePaths) *pppSelectedFilePaths = ppItemPointers;
    }

    dtk_free(pFilter);
    return DTK_DIALOG_RESULT_OK;
}

dtk_dialog_result dtk_show_save_file_dialog__win32(dtk_window* pParentWindow, dtk_save_file_dialog_options* pOptions, char** ppSelectedFilePath)
{
    dtk_assert(pOptions != NULL);

    // Limit to 32K for the moment. Too big for the stack? Consider allocating something larger on the heap if this becomes an issue.
    char filePath[32768];
    if (pOptions->pDefaultPath != NULL) {
        dtk_strcpy_s(filePath, sizeof(filePath), dtk_path_file_name(pOptions->pDefaultPath));
    } else {
        filePath[0] = '\0';
    }

    // Filter.
    char* pFilter = dtk_convert_file_dialog_filters_to_win32(pOptions->ppExtensionFilters);
    if (pFilter == NULL) {
        return DTK_OUT_OF_MEMORY;
    }

    OPENFILENAMEA ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = (pParentWindow == NULL) ? NULL : (HWND)pParentWindow->win32.hWnd;
    ofn.lpstrFile = filePath;
    ofn.nMaxFile = sizeof(filePath);
    ofn.lpstrFilter = pFilter;
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
    if (!GetSaveFileNameA(&ofn)) {
        dtk_free(pFilter);
        return DTK_DIALOG_RESULT_CANCEL;
    }

    dtk_free(pFilter);

    // Output.
    size_t selectedFilePathLen = strlen(filePath);
    char* pSelectedFilePath = (char*)dtk_malloc(selectedFilePathLen + 1);
    if (pSelectedFilePath == NULL) {
        return DTK_OUT_OF_MEMORY;    // Out of memory.
    }

    dtk_strcpy_s(pSelectedFilePath, selectedFilePathLen+1, filePath);

    if (ppSelectedFilePath) {
        *ppSelectedFilePath = pSelectedFilePath;
    }

    return DTK_DIALOG_RESULT_OK;
}
#endif



///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//
// GTK
//
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#ifdef DTK_GTK
dtk_dialog_result dtk_message_box__gtk(dtk_window* pParentWindow, const char* text, const char* title, dtk_dialog_buttons buttons)
{
    GtkWidget* pDialog = gtk_message_dialog_new((pParentWindow == NULL) ? NULL : GTK_WINDOW(pParentWindow->gtk.pWidget), GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_OTHER, GTK_BUTTONS_NONE, "%s", text);
    gtk_window_set_title(GTK_WINDOW(pDialog), title);

    // Buttons.
    switch (buttons)
    {
        case DTK_DIALOG_BUTTONS_OKCANCEL:
        {
            gtk_dialog_add_button(GTK_DIALOG(pDialog), "OK", DTK_DIALOG_RESULT_OK);
            gtk_dialog_add_button(GTK_DIALOG(pDialog), "Cancel", DTK_DIALOG_RESULT_CANCEL);
        } break;

        case DTK_DIALOG_BUTTONS_YESNO:
        {
            gtk_dialog_add_button(GTK_DIALOG(pDialog), "Yes", DTK_DIALOG_RESULT_YES);
            gtk_dialog_add_button(GTK_DIALOG(pDialog), "No", DTK_DIALOG_RESULT_NO);
        } break;

        case DTK_DIALOG_BUTTONS_YESNOCANCEL:
        {
            gtk_dialog_add_button(GTK_DIALOG(pDialog), "Yes", DTK_DIALOG_RESULT_YES);
            gtk_dialog_add_button(GTK_DIALOG(pDialog), "No", DTK_DIALOG_RESULT_NO);
            gtk_dialog_add_button(GTK_DIALOG(pDialog), "Cancel", DTK_DIALOG_RESULT_CANCEL);
        } break;

        case DTK_DIALOG_BUTTONS_OK:
        default: 
        {
            gtk_dialog_add_button(GTK_DIALOG(pDialog), "OK", DTK_DIALOG_RESULT_OK);
        } break;
    }

    dtk_dialog_result dialogResult = (dtk_dialog_result)gtk_dialog_run(GTK_DIALOG(pDialog));
    gtk_widget_destroy(pDialog);

    return dialogResult;
}

dtk_dialog_result dtk_show_color_picker_dialog__gtk(dtk_context* pTK, dtk_window* pOwnerWindow, dtk_color initialColor, dtk_color* pColorOut)
{
    GtkWidget* dialog = gtk_color_chooser_dialog_new(NULL, (pOwnerWindow == NULL) ? NULL : GTK_WINDOW(pOwnerWindow->gtk.pWidget));
    if (dialog == NULL) {
        return DTK_DIALOG_RESULT_CANCEL;
    }

    GdkRGBA rgba;
    rgba.red   = initialColor.r / 255.0;
    rgba.green = initialColor.g / 255.0;
    rgba.blue  = initialColor.b / 255.0;
    rgba.alpha = initialColor.a / 255.0;
    gtk_color_chooser_set_rgba(GTK_COLOR_CHOOSER(dialog), &rgba);

    gint result = gtk_dialog_run(GTK_DIALOG(dialog));

    gtk_color_chooser_get_rgba(GTK_COLOR_CHOOSER(dialog), &rgba);
    pColorOut->r = (uint8_t)(rgba.red   * 255);
    pColorOut->g = (uint8_t)(rgba.green * 255);
    pColorOut->b = (uint8_t)(rgba.blue  * 255);
    pColorOut->a = (uint8_t)(rgba.alpha * 255);

    gtk_widget_destroy(dialog);

    if (result == GTK_RESPONSE_OK) {
        return DTK_DIALOG_RESULT_OK;
    } else {
        return DTK_DIALOG_RESULT_CANCEL;
    }
}

dtk_dialog_result dtk_show_font_picker_dialog__gtk(dtk_context* pTK, dtk_window* pOwnerWindow, const dtk_font_desc* pDefaultFontDesc, dtk_font_desc* pDescOut)
{
    (void)pDefaultFontDesc;

    GtkWidget* dialog = gtk_font_chooser_dialog_new(NULL, (pOwnerWindow == NULL) ? NULL : GTK_WINDOW(pOwnerWindow->gtk.pWidget));
    if (dialog == NULL) {
        return DTK_ERROR;
    }

    gint result = gtk_dialog_run(GTK_DIALOG(dialog));

    gchar* pangoFontStr = gtk_font_chooser_get_font(GTK_FONT_CHOOSER(dialog));
    if (pangoFontStr == NULL) {
        gtk_widget_destroy(dialog);
        return DTK_ERROR;
    }

    PangoFontDescription* pPangoDesc = pango_font_description_from_string(pangoFontStr);
    if (pPangoDesc != NULL) {
        dtk_strcpy_s(pDescOut->family, sizeof(pDescOut->family), pango_font_description_get_family(pPangoDesc));

        gint size = pango_font_description_get_size(pPangoDesc);
        if (size > 0) {
            if (pango_font_description_get_size_is_absolute(pPangoDesc)) {
                pDescOut->size = size;
            } else {
                pDescOut->size = (unsigned int)(size/PANGO_SCALE * (96.0/72.0));
            }
        }

        pDescOut->slant = dtk_font_slant_from_pango(pango_font_description_get_style(pPangoDesc));
        pDescOut->weight = dtk_font_weight_from_pango(pango_font_description_get_weight(pPangoDesc));

        pango_font_description_free(pPangoDesc);
    }

    gtk_widget_destroy(dialog);
    if (result == GTK_RESPONSE_OK) {
        return DTK_DIALOG_RESULT_OK;
    } else {
        return DTK_DIALOG_RESULT_CANCEL;
    }
}

dtk_dialog_result dtk_show_open_file_dialog__gtk(dtk_window* pParentWindow, dtk_open_file_dialog_options* pOptions, char*** pppSelectedFilePaths)
{
    GtkWidget* dialog = gtk_file_chooser_dialog_new("Save As", (pParentWindow == NULL) ? NULL : GTK_WINDOW(pParentWindow->gtk.pWidget), GTK_FILE_CHOOSER_ACTION_OPEN,
        "_Open",   GTK_RESPONSE_ACCEPT,
        "_Cancel", GTK_RESPONSE_CANCEL, NULL);
    if (dialog == NULL) {
        return DTK_ERROR;
    }

    gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(dialog), pOptions->multiSelect);

    if (!dtk_string_is_null_or_empty(pOptions->pDefaultPath)) {
        gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(dialog), pOptions->pDefaultPath);
    }


    // Filters.
    const char** ppNextFilter = pOptions->ppExtensionFilters;
    for (;;) {
        const char* pName = ppNextFilter[0];
        if (pName == NULL) {
            break;
        }

        const char* pExtensions = ppNextFilter[1];
        if (pExtensions == NULL) {
            break;
        }

        GtkFileFilter* pFilterGTK = gtk_file_filter_new();
        if (pFilterGTK == NULL) {
            break;
        }

        // Name.
        char* pPatternsStr = NULL;
        if (pExtensions[0] != '\0') {
            char* pTemp = dtk_make_string(pExtensions);
            char* pToken = strtok(pTemp, " ,");
            while (pToken != NULL) {
                pPatternsStr = dtk_append_stringf(pPatternsStr, "*.%s", pToken);
                pToken = strtok(NULL, " ,");
            }
            dtk_free_string(pTemp);
        } else {
            pPatternsStr = dtk_make_string("*.*");
        }

        char* pNameWithPatterns = dtk_make_stringf("%s (%s)", pName, pPatternsStr);
        gtk_file_filter_set_name(pFilterGTK, pNameWithPatterns);
        dtk_free_string(pNameWithPatterns);
        dtk_free_string(pPatternsStr);
        
        // Patterns.
        if (pExtensions[0] != '\0') {
            char* pTemp = dtk_make_string(pExtensions);
            char* pToken = strtok(pTemp, " ,");
            while (pToken != NULL) {
                char* pPattern = dtk_make_stringf("*.%s", pToken);
                gtk_file_filter_add_pattern(pFilterGTK, pPattern);
                dtk_free_string(pPattern);
                pToken = strtok(NULL, " ,");
            }
            dtk_free_string(pTemp);
        } else {
            gtk_file_filter_add_pattern(pFilterGTK, "*.*");
        }

        gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), pFilterGTK);
        ppNextFilter += 2;
    }


    dtk_dialog_result result = DTK_DIALOG_RESULT_CANCEL;
    GSList* pResultList = NULL;

    gint response = gtk_dialog_run(GTK_DIALOG(dialog));
    if (response == GTK_RESPONSE_ACCEPT) {
        pResultList = gtk_file_chooser_get_filenames(GTK_FILE_CHOOSER(dialog));
        if (pResultList == NULL) {
            result = DTK_ERROR;
            goto done;
        }

        // We build the list in two passes. The first pass just calculates the size of the output list so we know how much memory to allocated. The second pass extracts the data.
        dtk_uint32 fileCount = 0;
        size_t outputBufferSize = sizeof(char*);
        for (GSList* pNextItem = pResultList; pNextItem != NULL; pNextItem = pNextItem->next) {
            const char* pFilePath = (const char*)pNextItem->data;
            outputBufferSize += sizeof(char*) + strlen(pFilePath) + 1;  // +1 for null terminator.
            fileCount += 1;
        }

        char* pOutputBuffer = (char*)dtk_malloc(outputBufferSize);
        if (pOutputBuffer == NULL) {
            result = DTK_OUT_OF_MEMORY;
            goto done;
        }

        char** ppItemPointers = (char**)pOutputBuffer;
        size_t runningOffset = sizeof(char*) * (fileCount+1);    // +1 for the null-terminating item.

        dtk_uint32 iFile = 0;
        for (GSList* pNextItem = pResultList; pNextItem != NULL; pNextItem = pNextItem->next) {
            const char* pFilePath = (const char*)pNextItem->data;
            size_t filePathLen = strlen(pFilePath);

            ppItemPointers[iFile] = (pOutputBuffer + runningOffset);
            dtk_strcpy(ppItemPointers[iFile], pFilePath);

            runningOffset += filePathLen + 1;
            iFile += 1;
        }
        ppItemPointers[fileCount] = NULL; // <-- The null terminating item.

        if (pppSelectedFilePaths) *pppSelectedFilePaths = ppItemPointers;
        result = DTK_DIALOG_RESULT_OK;
    } else if (response == GTK_RESPONSE_CANCEL) {
        result = DTK_DIALOG_RESULT_CANCEL;
    } else {
        result = DTK_ERROR;
    }


done:
    for (GSList* pNextItem = pResultList; pNextItem != NULL; pNextItem = pNextItem->next) {
        g_free(pNextItem->data);
    }
    g_slist_free(pResultList);

    gtk_widget_destroy(dialog);
    return result;
}

dtk_dialog_result dtk_show_save_file_dialog__gtk(dtk_window* pParentWindow, dtk_save_file_dialog_options* pOptions, char** ppSelectedFilePath)
{
    GtkWidget* dialog = gtk_file_chooser_dialog_new("Save As", (pParentWindow == NULL) ? NULL : GTK_WINDOW(pParentWindow->gtk.pWidget), GTK_FILE_CHOOSER_ACTION_SAVE,
        "_Save",   GTK_RESPONSE_ACCEPT,
        "_Cancel", GTK_RESPONSE_CANCEL, NULL);
    if (dialog == NULL) {
        return DTK_ERROR;
    }

    gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(dialog), TRUE);

    if (!dtk_string_is_null_or_empty(pOptions->pDefaultPath)) {
        gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(dialog), pOptions->pDefaultPath);
    }

    // Filters.
    const char** ppNextFilter = pOptions->ppExtensionFilters;
    for (;;) {
        const char* pName = ppNextFilter[0];
        if (pName == NULL) {
            break;
        }

        const char* pExtensions = ppNextFilter[1];
        if (pExtensions == NULL) {
            break;
        }

        GtkFileFilter* pFilterGTK = gtk_file_filter_new();
        if (pFilterGTK == NULL) {
            break;
        }

        // Name.
        char* pPatternsStr = NULL;
        if (pExtensions[0] != '\0') {
            char* pTemp = dtk_make_string(pExtensions);
            char* pToken = strtok(pTemp, " ,");
            while (pToken != NULL) {
                pPatternsStr = dtk_append_stringf(pPatternsStr, "*.%s", pToken);
                pToken = strtok(NULL, " ,");
            }
            dtk_free_string(pTemp);
        } else {
            pPatternsStr = dtk_make_string("*.*");
        }

        char* pNameWithPatterns = dtk_make_stringf("%s (%s)", pName, pPatternsStr);
        gtk_file_filter_set_name(pFilterGTK, pNameWithPatterns);
        dtk_free_string(pNameWithPatterns);
        dtk_free_string(pPatternsStr);
        
        // Patterns.
        if (pExtensions[0] != '\0') {
            char* pTemp = dtk_make_string(pExtensions);
            char* pToken = strtok(pTemp, " ,");
            while (pToken != NULL) {
                char* pPattern = dtk_make_stringf("*.%s", pToken);
                gtk_file_filter_add_pattern(pFilterGTK, pPattern);
                dtk_free_string(pPattern);
                pToken = strtok(NULL, " ,");
            }
            dtk_free_string(pTemp);
        } else {
            gtk_file_filter_add_pattern(pFilterGTK, "*.*");
        }

        gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), pFilterGTK);
        ppNextFilter += 2;
    }

    dtk_dialog_result result = DTK_DIALOG_RESULT_CANCEL;

    gint response = gtk_dialog_run(GTK_DIALOG(dialog));
    if (response == GTK_RESPONSE_ACCEPT) {
        char* filenameGTK = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        if (filenameGTK == NULL) {
            result = DTK_OUT_OF_MEMORY;
            goto done;
        }

        // The output string needs to be freed with dtk_free() which means we need to make another copy using dtk_malloc().
        size_t selectedFilePathLen = strlen(filenameGTK);
        char* pSelectedFilePath = (char*)dtk_malloc(selectedFilePathLen + 1);
        if (pSelectedFilePath == NULL) {
            g_free(filenameGTK);
            result = DTK_OUT_OF_MEMORY;    // Out of memory.
            goto done;
        }

        dtk_strcpy_s(pSelectedFilePath, selectedFilePathLen+1, filenameGTK);

        if (ppSelectedFilePath) {
            *ppSelectedFilePath = pSelectedFilePath;
        }

        g_free(filenameGTK);
        result = DTK_DIALOG_RESULT_OK;
    } else if (response == GTK_RESPONSE_CANCEL) {   
        result = DTK_DIALOG_RESULT_CANCEL;
    } else {
        result = DTK_ERROR;
    }

done:
    gtk_widget_destroy(dialog);
    return result;
}
#endif


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


dtk_dialog_result dtk_message_box(dtk_window* pParentWindow, const char* text, const char* title, dtk_dialog_buttons buttons)
{
    if (text == NULL) text = "";
    if (title == NULL) title = "";

#ifdef DTK_WIN32
    return dtk_message_box__win32(pParentWindow, text, title, buttons);
#endif
#ifdef DTK_GTK
    return dtk_message_box__gtk(pParentWindow, text, title, buttons);
#endif
}

dtk_dialog_result dtk_show_color_picker_dialog(dtk_context* pTK, dtk_window* pOwnerWindow, dtk_color initialColor, dtk_color* pColorOut)
{
    if (pTK == NULL) return DTK_INVALID_ARGS;
   
#ifdef DTK_WIN32
    return dtk_show_color_picker_dialog__win32(pTK, pOwnerWindow, initialColor, pColorOut);
#endif
#ifdef DTK_GTK
    return dtk_show_color_picker_dialog__gtk(pTK, pOwnerWindow, initialColor, pColorOut);
#endif
}

dtk_dialog_result dtk_show_font_picker_dialog(dtk_context* pTK, dtk_window* pOwnerWindow, const dtk_font_desc* pDefaultFontDesc, dtk_font_desc* pDescOut)
{
    if (pTK == NULL) return DTK_INVALID_ARGS;

#ifdef DTK_WIN32
    return dtk_show_font_picker_dialog__win32(pTK, pOwnerWindow, pDefaultFontDesc, pDescOut);
#endif
#ifdef DTK_GTK
    return dtk_show_font_picker_dialog__gtk(pTK, pOwnerWindow, pDefaultFontDesc, pDescOut);
#endif
}

dtk_dialog_result dtk_show_open_file_dialog(dtk_window* pParentWindow, dtk_open_file_dialog_options* pOptions, char*** pppSelectedFilePaths)
{
    if (pppSelectedFilePaths != NULL) *pppSelectedFilePaths = NULL;

    dtk_open_file_dialog_options defaultOptions;
    dtk_zero_object(&defaultOptions);
    defaultOptions.multiSelect = DTK_FALSE;
    defaultOptions.fileMustExist = DTK_TRUE;
    if (pOptions == NULL) {
        pOptions = &defaultOptions;
    }

#ifdef DTK_WIN32
    return dtk_show_open_file_dialog__win32(pParentWindow, pOptions, pppSelectedFilePaths);
#endif
#ifdef DTK_GTK
    return dtk_show_open_file_dialog__gtk(pParentWindow, pOptions, pppSelectedFilePaths);
#endif
}

dtk_dialog_result dtk_show_save_file_dialog(dtk_window* pParentWindow, dtk_save_file_dialog_options* pOptions, char** ppSelectedFilePath)
{
    if (ppSelectedFilePath != NULL) *ppSelectedFilePath = NULL;

    dtk_save_file_dialog_options defaultOptions;
    dtk_zero_object(&defaultOptions);
    if (pOptions == NULL) {
        pOptions = &defaultOptions;
    }

#ifdef DTK_WIN32
    return dtk_show_save_file_dialog__win32(pParentWindow, pOptions, ppSelectedFilePath);
#endif
#ifdef DTK_GTK
    return dtk_show_save_file_dialog__gtk(pParentWindow, pOptions, ppSelectedFilePath);
#endif
}
