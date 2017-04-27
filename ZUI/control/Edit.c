#include <ZUI.h>


ZEXPORT ZuiAny ZCALL ZuiEditProc(ZuiInt ProcId, ZuiControl cp, ZuiEdit p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3) {
    switch (ProcId)
    {
    case Proc_OnEvent: {
        TEventUI *event = (TEventUI *)Param1;
        switch (event->Type)
        {
        case ZEVENT_TIMER: {
            p->cur_type = !p->cur_type;
            ZuiControlInvalidate(cp, TRUE);
            break;
        }
        case ZEVENT_MOUSELEAVE: {
            p->m_uState &= ~ZSTATE_HOT;
            ZuiControlInvalidate(cp, TRUE);
            break;
        }
        case ZEVENT_MOUSEENTER: {
            p->m_uState |= ZSTATE_HOT;
            ZuiControlInvalidate(cp, TRUE);
            return;
        }
        case ZEVENT_SETCURSOR: {
            SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_IBEAM)));
            return;
        }
        case ZEVENT_LBUTTONDOWN: {
            ZuiControlInvalidate(cp, TRUE);
            break;
        }
        case ZEVENT_LBUTTONUP: {
            ZuiControlInvalidate(cp, TRUE);
            break;
        }
        case ZEVENT_MOUSEMOVE: {

            break;
        }
        case ZEVENT_KILLFOCUS: {
            ZuiPaintManagerKillTimer_Id(cp, 1000);//关闭时钟
            ZuiPaintManagerSetImeStatus(cp->m_pManager, FALSE);//关闭输入法
            p->m_uState &= ~ZSTATE_FOCUSED;//取消焦点状态
            p->cur_type = FALSE;//关闭光标
            ZuiControlInvalidate(cp, TRUE);
            break;
        }
        case ZEVENT_SETFOCUS: {
            p->m_uState |= ZSTATE_FOCUSED;//开启焦点状态
            ZuiPaintManagerSetTimer(cp, 1000, 600);//开启光标时钟
            ZuiPaintManagerSetImeStatus(cp->m_pManager, TRUE);//打开输入法
            ZuiControlInvalidate(cp, TRUE);
            break;
        }
        case ZEVENT_CHAR: {
            //字符输入
            if (event->wParam == '\b') {
                //删除符
                ZuiControlCall(Proc_Edit_Delete, cp, NULL, NULL, NULL);
            }
            else if (event->wParam == '\r') {
                //回车
                ZuiEditProc(Proc_Edit_AddLine, cp, p, NULL, NULL, NULL);//添加一行
                p->write_pos.x = -1;
                p->write_pos.y++;//移动到新行
                ZuiControlNeedUpdate(cp);//重新布局
                ZuiControlInvalidate(cp, TRUE);
            }
            else {
                ZuiControlCall(Proc_Edit_AddChar, cp, event->wParam, NULL, NULL);//在当前读写位置添加一个字符
            }
            break;
        }
        case ZEVENT_KEYDOWN: {
            //按下按键
            if (event->wParam == VK_LEFT) {
                ZuiControlCall(Proc_Edit_MovePos, cp, -1, NULL, NULL);//将读写位置左移一个
            }
            else if (event->wParam == VK_RIGHT)
            {
                ZuiControlCall(Proc_Edit_MovePos, cp, 1, NULL, NULL);//将读写位置右移一个
            }
            else if (event->wParam == VK_UP)
            {
                ZuiControlCall(Proc_Edit_MoveLine, cp, -1, NULL, NULL);//将读写位置上移一行
            }
            if (event->wParam == VK_DOWN) {
                ZuiControlCall(Proc_Edit_MoveLine, cp, 1, NULL, NULL);//将读写位置下移一行
            }

            break;
        }
        default:
            break;
        }
        break;
    }
    case Proc_OnPaint: {
        //画背景色
        ZuiControlCall(Proc_OnPaintBkColor, cp, Param1, Param2, NULL);

        //画对象
        ZuiControlCall(Proc_Edit_OnPaintObject, cp, Param1, Param2, NULL);

        //画光标
        if (p->cur_type)
            ZuiControlCall(Proc_Edit_OnPaintCur, cp, Param1, Param2, NULL);

        //画边框
        ZuiControlCall(Proc_OnPaintBorder, cp, Param1, Param2, NULL);

        return 0;
    }
    case Proc_SetPos: {
        //通知父容器调整布局
        ZuiLayoutProc(ProcId, cp, p->old_udata, Param1, Param2, Param3);
        ZRect *rc = &cp->m_rcItem;
        ZPointR outpos = { 0,rc->top };
        for (size_t it1 = 0; it1 < p->line_data->count; it1++)//行
        {
            ZuiReal height = 0;//保存行高
            ZuiEditLine line = p->line_data->data[it1];//得到行对象
            outpos.x = rc->left + 2;
            for (size_t it2 = 0; it2 < line->m_array->count; it2++)
            {
                ZuiEditObject eo = line->m_array->data[it2];
                if (eo->type == ZEOT_TXT) {
                    ZSizeR sz = { 0 };//用来保存单个字符的大小
                    ZuiEditObjectText ot = (ZuiEditObjectText)eo;
                    ot->obj.size.cx = 0;
                    for (size_t i = 0; i < ot->write_len; i++)
                    {
                        ZuiMeasureTextSize(ot->font, ot->buf[i], &sz);//测量单个字符的大小

                        ot->obj.size.cx += sz.cx;//累加对象宽度
                        ot->out_pt[i].x = outpos.x;
                        outpos.x += sz.cx;
                        ot->out_pt[i].y = outpos.y + sz.cy;
                        ot->obj.size.cy = MAX(ot->obj.size.cy, sz.cy);//计算对象高度.最高的字符高度为准
                    }
                    p->cur_pos.x = ot->obj.size.cx;
                }

                height = MAX(height, eo->size.cy);//得到行高,最高的行高为准
            }
            line->rc.top = outpos.y;
            outpos.y += height;
            line->rc.bottom = outpos.y;
        }
        //布局完毕计算光标位置
        ZuiControlCall(Proc_Edit_CalcCurPos, cp, NULL, NULL, NULL);
        return;
    }
    case Proc_Edit_OnPaintObject: {
        ZuiGraphics gp = (ZuiGraphics)Param1;
        ZRect *rc = &cp->m_rcItem;

        //画对象
        for (size_t it1 = 0; it1 < p->line_data->count; it1++)//行
        {
            ZuiEditLine line = p->line_data->data[it1];
            for (size_t it2 = 0; it2 < line->m_array->count; it2++)
            {
                ZuiEditObject eo = line->m_array->data[it2];
                if (eo->type == ZEOT_TXT) {
                    ZuiEditObjectText ot = (ZuiEditObjectText)eo;
                    ZuiDrawStringPt(gp, ot->font, ARGB(255,255,0,0),ot->buf, ot->write_len, ot->out_pt);
                }
            }
        }
        return 0;
    }
    case Proc_Edit_OnPaintCur: {
        ZuiGraphics gp = (ZuiGraphics)Param1;
        ZRect *rc = &cp->m_rcItem;
        //画光标
        ZuiDrawLine(gp, ARGB(255, 0, 0, 0), p->cur_pos.x, p->cur_pos.y, p->cur_pos.x, p->cur_pos.y + p->cur_height, 1);
        return;
    }
    case Proc_OnPaintBorder: {
        ZuiGraphics gp = (ZuiGraphics)Param1;
        ZRect *rc = &cp->m_rcItem;

        if ((p->m_uState & ZSTATE_HOT) != 0)
        {
            ZuiDrawRect(gp, ARGB(200, 0, 0, 0), rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top, 1);//鼠标悬停
        }
        else if (cp->m_bFocused)
        {
            ZuiDrawRect(gp, ARGB(200, 34, 255, 255), rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top, 1);//焦点
        }
        else
        {
            ZuiDrawRect(gp, ARGB(200, 0, 30, 255), rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top, 1);//通常
        }
        return 0;
    }
    case Proc_Edit_CalcCurPos: {
        //获取输入行
        ZuiEditLine line = darray_getat(p->line_data, p->write_pos.y);
        if (!line)
            return 0;
        p->cur_pos.y = line->rc.top + 2;
        //获取输入对象
        ZuiEditObject eo = NULL;
        if (darray_len(line->m_array) == 0 || p->write_pos.x < 0) {
            //当前行没有输入对象,或当前编辑框读写位置在起始位置
            p->cur_pos.x = cp->m_rcItem.left+1;
        }
        else {
            //从当前读写位置得到对象
            ZuiEditObject eo = darray_getat(line->m_array, p->write_pos.x);
            if (eo->type == ZEOT_TXT) {
                //当前对象为文本对象
                ZuiEditObjectText ot = (ZuiEditObjectText)eo;
                ZSizeR sz = { 0 };
                if (ot->write_pos < 0) {
                    //起始位置
                    p->cur_pos.x = cp->m_rcItem.left+1;
                    
                }
                else {
                    ZuiMeasureTextSize(ot->font, ot->buf[ot->write_pos], &sz);
                    //更具当前对象的输入位置得到字符位置
                    p->cur_pos.x = ot->out_pt[ot->write_pos].x + sz.cx;
                }
                p->cur_height = eo->size.cy;
            }
        }

        return 0;
    }
    case Proc_Edit_Delete: {
        //退格
        //获取输入行
        ZuiEditLine line = darray_getat(p->line_data, p->write_pos.y);
        if (line) {
            if (darray_len(line->m_array) == 0) {
                //当前行没有输入对象,退行
                return ZuiControlCall(Proc_Edit_MoveLine, cp, -1, 1, NULL);
            }
            else {
                //从当前读写位置得到对象
                ZuiEditObject eo = darray_getat(line->m_array, p->write_pos.x);
                if (eo->type == ZEOT_TXT) {
                    //当前读写对象为文本对象
                    ZuiEditObjectText ot = (ZuiEditObjectText)eo;
                    if (ot->write_pos + 1 == ot->write_len) {
                        //读写位置在尾部
                        ot->write_pos--;//先移动读写位置
                        ot->buf[ot->write_pos + 1] = '\0';
                        ot->write_len--;//写入长度减一
                    }
                    if (ot->write_len == 0)
                    {
                        //内容被删除完毕,删除这个对象
                        darray_delete(line->m_array, darray_find(line->m_array, eo));
                        //移动编辑框读写位置
                        p->write_pos.x--;
                    }

                }


            }


            ZuiControlNeedUpdate(cp);//重新布局
            ZuiControlInvalidate(cp, TRUE);
            return TRUE;

        }
        return FALSE;
    }
    case Proc_Edit_DeleteAll: {
        for (size_t i = 0; i < p->line_data->count; i++)
        {
            ZuiEditLine line = darray_getat(p->line_data, i);
            for (size_t j = 0; j < line->m_array->count; j++)
            {
                ZuiEditObject eo = darray_getat(line->m_array, j);
                if (eo->type == ZEOT_TXT) {
                    //当前读写对象为文本对象
                    ZuiEditObjectText ot = (ZuiEditObjectText)eo;
                    ZuiFree(ot->buf);
                    ZuiFree(ot->out_pt);
                }
            }
            darray_destroy(line->m_array);
        }
        //清空数组
        darray_empty(p->line_data);

        //添加一行 至少保留一行
        ZuiEditProc(Proc_Edit_AddLine, cp, p, NULL, NULL, NULL);

        //重置变量
        //默认光标
        p->cur_pos.x = 1;
        p->cur_height = 24;

        //初始读写位置
        p->write_pos.x = -1;
        p->write_pos.y = 0;//第一行

        return FALSE;
    }
    case Proc_Edit_MovePos: {
        //移动光标(读写位置)
        if (Param1 == 0)
            return FALSE;
        //获取输入行
        ZuiEditLine line = darray_getat(p->line_data, p->write_pos.y);
        if (line) {
            if (p->write_pos.x < 0 && darray_len(line->m_array) != 0) {
                //当前编辑框读写位置在起始位置,且当前行有对象
                if (((ZuiInt)Param1) < 0) {
                    //左移移动到上一行的结尾
                    return ZuiControlCall(Proc_Edit_MoveLine, cp, -1, 1, NULL);
                }
                else
                {
                    //右移移动到下一个对象
                    p->write_pos.x++;
                }
            }

            if (darray_len(line->m_array) == 0) {
                //当前行没有输入对象,或当前编辑框读写位置在起始位置
                if (((ZuiInt)Param1) < 0) {
                    //左移移动到上一行的结尾
                    return ZuiControlCall(Proc_Edit_MoveLine, cp, -1, 1, NULL);
                }
                else
                {
                    //右移移动到下一行的开始
                    return ZuiControlCall(Proc_Edit_MoveLine, cp, 1, -1, NULL);
                }
            }
            else {
                //从当前读写位置得到对象
                ZuiEditObject eo = darray_getat(line->m_array, p->write_pos.x);
                if (eo->type == ZEOT_TXT) {
                    //当前读写对象为文本对象
                    ZuiEditObjectText ot = (ZuiEditObjectText)eo;
                    //移动读写位置
                    if (((ZuiInt)Param1) < 0) {
                        //左移
                        if (ot->write_pos < 0) {
                            //超出了当前对象
                            if (p->write_pos.x == 0) {
                                //已经是在当前行的头对象
                                //左移移动到上一行的尾部
                                return ZuiControlCall(Proc_Edit_MoveLine, cp, -1, 1, NULL);
                            }
                            else {
                                //当前行左边还有对象
                                p->write_pos.x--;
                                ot->write_pos = -1;//重置当前对象的写入位置
                            }
                        }
                        else
                            ot->write_pos--;
                    }
                    else
                    {
                        //右移
                        if (ot->write_pos + 1 == ot->write_len)
                        {
                            //超出了当前对象
                            if (p->write_pos.x + 1 == darray_len(line->m_array)) {
                                //已经是在当前行的行尾
                                //右移移动到下一行的开始
                                return ZuiControlCall(Proc_Edit_MoveLine, cp, 1, -1, NULL);
                            }
                            else {
                                //当前行后面还有对象
                                p->write_pos.x++;
                                ot->write_pos = -1;//重置当前对象的写入位置
                            }
                        }
                        else
                            ot->write_pos++;
                    }
                }


            }
            //重新计算光标位置
            ZuiControlCall(Proc_Edit_CalcCurPos, cp, NULL, NULL, NULL);
            if (cp->m_bFocused)
                p->cur_type = TRUE;//焦点状态,移动读写位置后立即显示光标位置
            ZuiControlInvalidate(cp, TRUE);
            return TRUE;
        }
        return FALSE;
    }
    case Proc_Edit_MovePosLineEnd: {
        
        return FALSE;
    }
    case Proc_Edit_MoveLine: {
        //在行间移动读写位置,参数二指定新读写位置在行头或者行尾,空为自动计算,负数为头部,正数为尾部
        if (((ZuiInt)Param1) < 0) {
            //上移
            if (p->write_pos.y > 0)
                p->write_pos.y--;
        }
        else
        {
            //下移
            if (p->write_pos.y + 1 < darray_len(p->line_data))
                p->write_pos.y++;
        }


        //重新计算光标位置
        ZuiControlCall(Proc_Edit_CalcCurPos, cp, NULL, NULL, NULL);
        if (cp->m_bFocused)
            p->cur_type = TRUE;//焦点状态,移动读写位置后立即显示光标位置
        ZuiControlInvalidate(cp, TRUE);
        return FALSE;
    }
    case Proc_Edit_AddLine: {
        //添加一行
        ZuiEditLine line = (ZuiEditLine)ZuiMalloc(sizeof(ZEditLine));
        if (line) {
            line->m_array = darray_create();
            return darray_append(p->line_data, line);
        }
        return FALSE;
    }
    case Proc_Edit_AddChar: {
        //添加一个字符
        //获取输入行
        ZuiEditLine line = darray_getat(p->line_data, p->write_pos.y);
        if (line) {
            //获取文本输入对象
            ZuiEditObjectText ot = NULL;
            if (darray_len(line->m_array) == 0)
            {
                //当前行没有类容,添加一个对象
                ot = (ZuiEditObjectText)ZuiMalloc(sizeof(ZEditObjectText));
                memset(ot, 0, sizeof(ZEditObjectText));
                ot->obj.type = ZEOT_TXT;
                ot->font = p->font;
                ot->buflen = Proc_Edit_ObjectTextBufLen;
                ot->buf = (ZuiText)ZuiMalloc(sizeof(_ZuiText)*Proc_Edit_ObjectTextBufLen);
                ot->write_pos = -1;
                ot->buf[0] = '\0';
                ot->out_pt = (ZuiPointR)ZuiMalloc(sizeof(ZPointR)*Proc_Edit_ObjectTextBufLen);
                memset(ot->out_pt, 0, sizeof(ZPointR)*Proc_Edit_ObjectTextBufLen);
                darray_append(line->m_array, ot);//插入到行尾
                p->write_pos.x++;//移动读写位置
            }
            else {
                //从当前读写位置得到对象
                ZuiEditObject eo = darray_getat(line->m_array, p->write_pos.x);
                if (eo->type == ZEOT_TXT)
                    ot = (ZuiEditObjectText)eo;
                else
                {
                    //当前对象不是文本对象,在当前位置插入一个文本对象
                    ot = (ZuiEditObjectText)ZuiMalloc(sizeof(ZEditObjectText));
                    memset(ot, 0, sizeof(ZEditObjectText));
                    ot->obj.type = ZEOT_TXT;
                    ot->font = p->font;
                    ot->buflen = Proc_Edit_ObjectTextBufLen;
                    ot->buf = (ZuiText)ZuiMalloc(sizeof(_ZuiText)*Proc_Edit_ObjectTextBufLen);
                    ot->write_pos = -1;
                    ot->buf[0] = '\0';
                    darray_insert(line->m_array, p->write_pos.x, ot);//插入到读写位置
                    p->write_pos.x++;//移动读写位置
                }

            }
            if (!ot)
                return FALSE;
            //得到文本对象
            ot->write_pos++;//先移动读写位置
            if (ot->write_pos == ot->write_len) {
                //尾部直接写入
                ot->buf[ot->write_pos] = (_ZuiText)Param1;
                ot->buf[ot->write_pos + 1] = '\0';
            }
            else {


            }
            ot->write_len++;//写入长度加一
            if (!Param2) {
                ZuiControlNeedUpdate(cp);//重新布局
                ZuiControlInvalidate(cp, TRUE);
            }
            return TRUE;

        }
        return FALSE;
    }
    case Proc_Edit_AddTxt: {
        //添加一个字符串
        ZuiText s = Param1;
        while (*s)
        {
            ZuiControlCall(Proc_Edit_AddChar, cp, *s, NULL, NULL);//先清空全部
            s++;
        }
        return FALSE;
    }
    case Proc_SetText: {
        ZuiControlCall(Proc_Edit_DeleteAll, cp, NULL, NULL, NULL);//先清空全部
        ZuiControlCall(Proc_Edit_AddTxt, cp, Param1, NULL, NULL);//插入字符串
        return;
    }
    case Proc_GetText: {
        //获取输入行
        ZuiEditLine line = darray_getat(p->line_data, p->write_pos.y);
        if (line) {
            ZuiEditObject eo = darray_getat(line->m_array, p->write_pos.x);
            if (eo) {
                if (eo->type == ZEOT_TXT) {
                    //当前读写对象为文本对象
                    ZuiEditObjectText ot = (ZuiEditObjectText)eo;
                    return ot->buf;
                }
            }
        }
        return NULL;
    }
    case Proc_SetAttribute: {
        if (wcscmp(Param1, L"font") == 0) ZuiControlCall(Proc_Label_SetFont, cp, ZuiResDBGetRes(Param2, ZREST_FONT), NULL, NULL);
        if (wcscmp(Param1, L"align") == 0) {
            //横向对齐方式
            if (wcscmp(Param2, L"left") == 0) {
                p->m_uTextStyle &= ~(ZDT_CENTER | ZDT_RIGHT);
                p->m_uTextStyle |= ZDT_LEFT;
            }
            if (wcscmp(Param2, L"center") == 0) {
                p->m_uTextStyle &= ~(ZDT_LEFT | ZDT_RIGHT);
                p->m_uTextStyle |= ZDT_CENTER;
            }
            if (wcscmp(Param2, L"right") == 0) {
                p->m_uTextStyle &= ~(ZDT_LEFT | ZDT_CENTER);
                p->m_uTextStyle |= ZDT_RIGHT;
            }
        }
        else if (wcscmp(Param1, L"valign") == 0) {
            //纵向对齐方式
            if (wcscmp(Param2, L"top") == 0) {
                p->m_uTextStyle &= ~(ZDT_BOTTOM | ZDT_VCENTER | ZDT_WORDBREAK);
                p->m_uTextStyle |= (ZDT_TOP | ZDT_SINGLELINE);
            }
            if (wcscmp(Param2, L"vcenter") == 0) {
                p->m_uTextStyle &= ~(ZDT_TOP | ZDT_BOTTOM | ZDT_WORDBREAK);
                p->m_uTextStyle |= (ZDT_VCENTER | ZDT_SINGLELINE);
            }
            if (wcscmp(Param2, L"bottom") == 0) {
                p->m_uTextStyle &= ~(ZDT_TOP | ZDT_VCENTER | ZDT_WORDBREAK);
                p->m_uTextStyle |= (ZDT_BOTTOM | ZDT_SINGLELINE);
            }
        }
        else if (wcscmp(Param1, L"textcolor") == 0) {
            //字体颜色
            LPTSTR pstr = NULL;
            DWORD clrColor;
            while (*(wchar_t *)Param2 > L'\0' && *(wchar_t *)Param2 <= L' ') (wchar_t *)Param2 = CharNext((wchar_t *)Param2);
            if (*(wchar_t *)Param2 == L'#') (wchar_t *)Param2 = CharNext((wchar_t *)Param2);
            clrColor = _tcstoul((wchar_t *)Param2, &pstr, 16);
            ZuiControlCall(Proc_Label_SetTextColor, cp, clrColor, NULL, NULL);
        }
        //if (_tcsicmp(pstrName, _T("readonly")) == 0) 
        //    //只读
        //    SetReadOnly(_tcsicmp(pstrValue, _T("true")) == 0);
        //else if (_tcsicmp(pstrName, _T("numberonly")) == 0)
        //    //只输入数字
        //    SetNumberOnly(_tcsicmp(pstrValue, _T("true")) == 0);
        //else if (_tcsicmp(pstrName, _T("password")) == 0) 
        //    //密码模式
        //    SetPasswordMode(_tcsicmp(pstrValue, _T("true")) == 0);
        //else if (_tcsicmp(pstrName, _T("passwordchar")) == 0) 
        //    //密码掩码
        //    SetPasswordChar(*pstrValue);
        //else if (_tcsicmp(pstrName, _T("maxchar")) == 0) 
        //    //最大输入长度
        //    SetMaxChar(_ttoi(pstrValue));
        
        //else if (_tcsicmp(pstrName, _T("normalimage")) == 0) SetNormalImage(pstrValue);
        //else if (_tcsicmp(pstrName, _T("hotimage")) == 0) SetHotImage(pstrValue);
        //else if (_tcsicmp(pstrName, _T("focusedimage")) == 0) SetFocusedImage(pstrValue);
        //else if (_tcsicmp(pstrName, _T("disabledimage")) == 0) SetDisabledImage(pstrValue);
        //else if (_tcsicmp(pstrName, _T("tipvalue")) == 0) SetTipValue(pstrValue);
        //else if (_tcsicmp(pstrName, _T("tipvaluecolor")) == 0) SetTipValueColor(pstrValue);
        //else if (_tcsicmp(pstrName, _T("nativetextcolor")) == 0) SetNativeEditTextColor(pstrValue);
        //else if (_tcsicmp(pstrName, _T("nativebkcolor")) == 0) {
        //    if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
        //    LPTSTR pstr = NULL;
        //    DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
        //    SetNativeEditBkColor(clrColor);
        //}
        break;
    }
    case Proc_GetImePoint: {
        ZPoint pt;
        pt.x = p->cur_pos.x - cp->m_rcItem.left + 1;
        pt.y = p->cur_pos.y -cp->m_rcItem.top+ 2;
        return &pt;
    }
    case Proc_OnInit: {
        break;
    }
    case Proc_GetControlFlags: {
        return ZFLAG_SETCURSOR;
    }
    case Proc_OnCreate: {
        p = (ZuiEdit)ZuiMalloc(sizeof(ZEdit));
        memset(p, 0, sizeof(ZEdit));
        //保存原来的回调地址,创建成功后回调地址指向当前函数
        p->old_udata = ZuiLayoutProc(Proc_OnCreate, cp, 0, 0, 0, 0);
        p->old_call = (ZCtlProc)&ZuiLayoutProc;
        p->font = ZuiCreateFont(L"微软雅黑", 12, FALSE, FALSE);

        p->line_data = darray_create();

        //添加一行 至少保留一行
        ZuiEditProc(Proc_Edit_AddLine, cp, p, NULL, NULL, NULL);

        //默认光标
        p->cur_pos.x = 1;
        p->cur_height = 24;

        //初始读写位置
        p->write_pos.x = -1;
        p->write_pos.y = 0;//第一行
        return p;
    }
    case Proc_OnDestroy: {
        ZCtlProc old_call = p->old_call;

        ZuiFree(p);

        return old_call(ProcId, cp, 0, Param1, Param2, Param3);
    }
    case Proc_GetObject:
        if (Param1 == Type_Edit)
            return (ZuiAny)p;
        break;
    case Proc_GetType:
        return (ZuiAny)Type_Edit;
    case Proc_CoreInit:
        return (ZuiAny)TRUE;
    case Proc_CoreUnInit:
        return (ZuiAny)TRUE;
    default:
        break;
    }
    return p->old_call(ProcId, cp, p->old_udata, Param1, Param2, Param3);
}




