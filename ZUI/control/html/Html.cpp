#include <ZUI.h>

#include "dw/core.hh"
#include "dw/textblock.hh"
#include "dw/listitem.hh"
#include "dw/zuiplatform.hh"
#include "dw/zuiviewport.hh"

using namespace dw;
using namespace dw::core;
using namespace dw::core::style;
using namespace dw::zui;

typedef struct _ZuiHtmlCore
{
    ZHtml zhtml;
    ZuiPlatform *platform;
    Layout *layout;

}*ZuiHtmlCore, ZHtmlCore;

ZEXPORT ZuiAny ZCALL ZuiHtmlProc(ZuiInt ProcId, ZuiControl cp, ZuiHtml p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3) {
    switch (ProcId)
    {
    case Proc_CoreInit: {
        return (ZuiAny)TRUE;
        break;
    }
    case Proc_OnCreate: {
        p = (ZuiHtml)malloc(sizeof(ZHtmlCore));
        memset(p, 0, sizeof(ZHtmlCore));
        //保存原来的回调地址,创建成功后回调地址指向当前函数
        p->old_call = cp->call;
        p->cp = cp;
        ZuiPlatform *platform = new ZuiPlatform();
        Layout *layout = new Layout(platform);


        ZuiViewport *viewport = new ZuiViewport(0, 0, 200, 300);
        layout->attachView(viewport);

        StyleAttrs styleAttrs;
        styleAttrs.initValues();
        styleAttrs.margin.setVal(5);
        styleAttrs.borderWidth.setVal(2);
        styleAttrs.setBorderColor(Color::create(layout, 0xffffff));
        styleAttrs.setBorderStyle(BORDER_INSET);
        styleAttrs.padding.setVal(5);

        FontAttrs fontAttrs;
        fontAttrs.name = "Bitstream Charter";
        fontAttrs.size = 14;
        fontAttrs.weight = 400;
        fontAttrs.style = FONT_STYLE_NORMAL;
        fontAttrs.letterSpacing = 0;
        fontAttrs.fontVariant = FONT_VARIANT_NORMAL;
        styleAttrs.font = dw::core::style::Font::create(layout, &fontAttrs);

        styleAttrs.color = Color::create(layout, 0x000000);
        styleAttrs.backgroundColor = Color::create(layout, 0xffffff);

        Style *widgetStyle1 = Style::create(layout, &styleAttrs);

        styleAttrs.backgroundColor = Color::create(layout, 0xffff80);
        styleAttrs.margin.setVal(0);
        styleAttrs.borderWidth.setVal(1);
        styleAttrs.setBorderColor(Color::create(layout, 0x4040ff));
        styleAttrs.setBorderStyle(BORDER_SOLID);
        styleAttrs.padding.setVal(1);

        Style *widgetStyle2 = Style::create(layout, &styleAttrs);

        Textblock *textblock1 = new Textblock(false);
        textblock1->setStyle(widgetStyle1);
        layout->setWidget(textblock1);

        widgetStyle1->unref();

        styleAttrs.borderWidth.setVal(0);
        styleAttrs.padding.setVal(0);
        styleAttrs.backgroundColor = NULL;
        styleAttrs.cursor = CURSOR_TEXT;

        Style *wordStyle = Style::create(layout, &styleAttrs);

        const char *words1[] = { "Some", "random", "text.", NULL };
        const char *words2[] = { "A", "nested", "paragraph.", NULL };

        for (int i = 0; words1[i]; i++) {
            if (i != 0)
                textblock1->addSpace(wordStyle);
            textblock1->addText(words1[i], wordStyle);
        }

        for (int i = 0; i < 10; i++) {
            textblock1->addParbreak(0, wordStyle);

            Textblock *textblock2 = new Textblock(false);
            textblock1->addWidget(textblock2, widgetStyle2);

            for (int j = 0; words2[j]; j++) {
                if (j != 0)
                    textblock2->addSpace(wordStyle);
                textblock2->addText(words2[j], wordStyle);
            }

            textblock2->flush();
        }

        textblock1->flush();


        return p;
        break;
    }
    case Proc_OnDestroy: {
        break;
    }
    case Proc_OnSize: {
        break;
    }
    case Proc_OnEvent: {
        TEventUI *event = (TEventUI *)Param1;
        switch (event->Type)
        {
        case ZEVENT_TIMER: {
            break;
        }
        case ZEVENT_SETFOCUS:
            break;
        case ZEVENT_KILLFOCUS:
            break;
        case ZEVENT_CHAR: {
            break;
        }
        case ZEVENT_KEYUP: {
            break;
        }
        case ZEVENT_KEYDOWN: {
            break;
        }
        case ZEVENT_LBUTTONDOWN:
        case ZEVENT_LBUTTONUP:
        case ZEVENT_RBUTTONDOWN:
        case ZEVENT_LDBLCLICK:
        case ZEVENT_MOUSEMOVE: {
            break;
        }
        case ZEVENT_SCROLLWHEEL: {
            break;
        }
        case ZEVENT_SETCURSOR:
            break;
        default:
            break;
        }
        break;
    }
    case Proc_OnPaint: {
        ZuiGraphics gp = (ZuiGraphics)Param1;
        RECT *rc = &cp->m_rcItem;

        break;
    }
    case Proc_SetPos: {
        
        break;
    }
    case Proc_GetImePoint: {
        ZPoint pt;
        pt.x = 0;
        pt.y = 0;
        return &pt;
        break;
    }
    case Proc_OnInit: {

        break;
    }
    case Proc_GetControlFlags: {
        return (ZuiAny)ZFLAG_SETCURSOR;
        break;
    }
    default:
        break;
    }
    return p->old_call(ProcId, cp, 0, Param1, Param2, Param3);
}

