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
        ZPointR outpos = { rc->left + 2,rc->top };
        for (size_t it1 = 0; it1 < p->line_data->count; it1++)//行
        {
            ZuiReal height = 0;//保存行高
            ZuiEditLine line = p->line_data->data[it1];
            for (size_t it2 = 0; it2 < line->m_array->count; it2++)
            {
                ZuiEditObject eo = line->m_array->data[it2];
                if (eo->type == ZEOT_TXT) {
                    ZSizeR sz = { 0 };//用来保存单个字符的大小
                    ZuiEditObjectText ot = (ZuiEditObjectText)eo;
                    ot->obj.size.cx = 0;
                    for (size_t i = 0; i < ot->write_len; i++)
                    {
                        ZuiMeasureTextSize(cp->m_pManager->m_hDcOffscreen, ot->sf, ot->buf[i], &sz);//测量单个字符的大小

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
                    ZuiDrawStringR(gp, ot->sf, ot->buf, ot->write_len, ot->out_pt);
                }
            }
        }
        return 0;
    }
    case Proc_Edit_OnPaintCur: {
        ZuiGraphics gp = (ZuiGraphics)Param1;
        ZRect *rc = &cp->m_rcItem;
        //画光标
        ZuiDrawLineR(gp, ARGB(255, 0, 0, 0), rc->left + p->cur_pos.x, rc->top + 2, rc->left + p->cur_pos.x, rc->top + p->cur_height - 4, 1);
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
        //获取输入对象
        ZuiEditObject eo = NULL;
        if (darray_len(line->m_array) == 0) {
            //当前行没有输入对象
            p->cur_pos.x = 2;
        }
        else {
            //从当前读写位置得到对象
            ZuiEditObject eo = darray_getat(line->m_array, p->write_pos.x);
            if (eo->type == ZEOT_TXT) {
                //当前对象为文本对象
                ZuiEditObjectText ot = (ZuiEditObjectText)eo;
                ZSizeR sz = { 0 };
                ZuiMeasureTextSize(cp->m_pManager->m_hDcOffscreen, ot->sf, ot->buf[ot->write_pos], &sz);
                //更具当前对象的输入位置得到字符位置
                p->cur_pos.x = ot->out_pt[ot->write_pos].x + sz.cx;

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

            //获取文本输入对象

            ZuiControlNeedUpdate(cp);//重新布局
            ZuiControlInvalidate(cp, TRUE);
            return TRUE;

        }
        return FALSE;
    }
    case Proc_Edit_MovePos: {
        //移动光标(读写位置)


        return FALSE;
    }
    case Proc_Edit_MoveLine: {
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
                ot->sf = p->sf;
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
                    ot->sf = p->sf;
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
            ZuiControlNeedUpdate(cp);//重新布局
            ZuiControlInvalidate(cp, TRUE);
            return TRUE;

        }
        return FALSE;
    }
    case Proc_Edit_AddTxt: {
        //添加一个字符串
        return FALSE;
    }
    case Proc_SetText: {

        return;
    }
    case Proc_SetAttribute: {
        break;
    }
    case Proc_GetImePoint: {
        ZPoint pt;
        pt.x = p->cur_pos.x + 1;
        pt.y = p->cur_pos.y + 2;
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
        p->sf = ZuiCreateStringFormat(L"微软雅黑", 12, ARGB(255, 255, 0, 0), ARGB(255, 255, 255, 255), 0);

        p->line_data = darray_create();

        //添加一行
        ZuiEditProc(Proc_Edit_AddLine, cp, p, NULL, NULL, NULL);

        //默认光标
        p->cur_pos.x = 2;
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




