#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <gem.h>

#define DEBUG
#ifdef DEBUG
#include "natfeats.h"
#define dbg(format, arg...) do { nf_printf("DEBUG: (%s):" format, __FUNCTION__, ##arg); } while (0)
#define out(format, arg...) do { nf_printf("" format, ##arg); } while (0)
#else
#define dbg(format, arg...) do { ; } while (0)
#endif /* DEBUG */

void open_window(short *whandle, OBJECT *dial, GRECT *wb, GRECT *ww)
{
    wind_calc(WC_BORDER, NAME | CLOSER | MOVER, 100, 100,
              dial[0].ob_width, dial[0].ob_height,
            &wb->g_x, &wb->g_y, &wb->g_w, &wb->g_h);
    wind_calc(WC_WORK, NAME | CLOSER | MOVER,
              wb->g_x, wb->g_y, wb->g_w, wb->g_h,
              &ww->g_x, &ww->g_y, &ww->g_w, &ww->g_h);
    *whandle = wind_create(NAME | CLOSER | MOVER,
                          wb->g_x, wb->g_y, wb->g_w, wb->g_h);
    wind_set_str(*whandle, WF_NAME, " Click Me ");

    wind_open(*whandle, wb->g_x, wb->g_y, wb->g_w, wb->g_h);

}

int main(void)
{
    short ap_id;
    static short whandle = -1;
    GRECT wb;
    GRECT ww;
    short evnt;
    bool quit = false;

    OBJECT dial[] =
    {
        /* a dialog object frame */
        {
            .ob_next = -1,
            .ob_head = 1,
            .ob_tail = 3,
            .ob_type = G_BOX,
            .ob_flags = OF_FL3DBAK,
            .ob_state = OS_NORMAL,
            .ob_spec = { 0L },
            /* can't use detailed ob_spec for 3D appearance
             * .ob_spec.obspec =
                (BFOBSPEC)
            {
                .framecol = G_BLACK,
                .textcol = G_BLACK,
                .interiorcol = G_LBLACK,
                .fillpattern = IP_HOLLOW,
            },*/
            .ob_x = 0,
            .ob_y = 0,
            .ob_width = 210,
            .ob_height = 130
        },
        /* with one single button in it */
        {
            .ob_next = 2,
            .ob_head = -1,
            .ob_tail = -1,
            .ob_type = G_BUTTON,
            .ob_flags = OF_SELECTABLE | OF_FL3DACT,
            .ob_state = OS_NORMAL,
            .ob_spec = { .free_string = " clickme "},
            .ob_x = 20,
            .ob_y = 20,
            .ob_width = 100,
            .ob_height = 16
        },
        {
            .ob_next = 3,
            .ob_head = -1,
            .ob_tail = -1,
            .ob_type = G_STRING,
            .ob_flags = OF_NONE,
            .ob_state = OS_NORMAL,
            .ob_spec = { .free_string = "not clicked "},
            .ob_x = 20,
            .ob_y = 50,
            .ob_width = 100,
            .ob_height = 16
        },
        {
            .ob_next = 0,
            .ob_head = -1,
            .ob_tail = -1,
            .ob_type = G_BUTTON,
            .ob_flags = OF_DEFAULT | OF_SELECTABLE | OF_FL3DACT | OF_EXIT,
            .ob_state = OS_NORMAL,
            .ob_spec = { .free_string = " EXIT " },
            .ob_x = 20,
            .ob_y = 80,
            .ob_width = 100,
            .ob_height = 16
        }
    };

    ap_id = appl_init();

    extern short _app;
    if (! _app)
    {
        menu_register(ap_id, "  Click me ");
    }
    else
    {
        graf_mouse(ARROW, NULL);
        open_window(&whandle, dial, &wb, &ww);
    }


    /*
     * parameter struct for evnt_multi_fast
     */
    EVMULT_IN evi =
    {
        .emi_flags = MU_MESAG | MU_BUTTON,
        .emi_bclicks = 1,
        .emi_bmask = 1,
        .emi_bstate = 1,
        .emi_m1leave = 0,
        .emi_m1 = {0, 0, 0, 0},
        .emi_m2leave = 0,
        .emi_m2 = {0, 0, 0, 0},
        .emi_tlow = 0, .emi_thigh = 0
    };

    do
    {
        short buf[8];
        GRECT wr;

        EVMULT_OUT evo;

        evnt = evnt_multi_fast(&evi, buf, &evo);

        if (evnt & MU_BUTTON)
        {
            short ob;
            short msg[8] = { WM_REDRAW, ap_id, 0, whandle, wb.g_x, wb.g_y, wb.g_w, wb.g_h };


            ob = objc_find(dial, ROOT, MAX_DEPTH, evo.emo_mouse.p_x, evo.emo_mouse.p_y);
            dbg("at (%d, %d) found object %d\r\n", evo.emo_mouse.p_x, evo.emo_mouse.p_y, ob);
            if (dial[ob].ob_flags & OF_SELECTABLE)
            {
                dbg("emo_events=%d, emo_mbutton=%d, emo_mclicks=%d, obj=%d\r\n", evo.emo_events, evo.emo_mbutton, evo.emo_mclicks, ob);
                /* an element of our object tree was clicked */
                if (evo.emo_mbutton == 1 && evo.emo_mclicks == 1)
                {
                    dial[ob].ob_state |= OS_SELECTED;
                    dial[2].ob_spec.free_string = "clicked";
                }
                else
                {
                    dial[ob].ob_state &= ~OS_SELECTED;
                    dial[2].ob_spec.free_string = "not clicked";
                }
                appl_write(ap_id, sizeof(msg), msg);

                evi.emi_bstate ^= 1;                             /* press recognised, wait for release */
            }
        }

        if (evnt & MU_MESAG)
        {
            switch (buf[0])
            {
                case AC_OPEN:
                    open_window(&whandle, dial, &wb, &ww);
                    break;

                case AC_CLOSE:
                    break;

                case WM_MOVED:
                    dbg("moved\r\n");
                    wind_set_grect(whandle, WF_CURRXYWH, (GRECT *) & buf[4]);
                    wind_get_grect(whandle, WF_CURRXYWH, &wb);
                    wind_get_grect(whandle, WF_WORKXYWH, &ww);
                    dial[0].ob_x = ww.g_x;
                    dial[0].ob_y = ww.g_y;
                    break;

                case WM_FULLED:
                    dbg("fulled\r\n");
                    break;

                case WM_TOPPED:
                    wind_set(whandle, WF_TOP, wb.g_x, wb.g_y, wb.g_w, wb.g_h);
                    dbg("topped\r\n");
                    break;

                case WM_REDRAW:
                    dbg("redraw\r\n");
                    wind_get_grect(whandle, WF_CURRXYWH, &wb);
                    dbg("wb=(%d, %d, %d, %d\r\n", wb.g_x, wb.g_y, wb.g_w, wb.g_h);
                    wind_get_grect(whandle, WF_WORKXYWH, &ww);
                    dbg("ww=(%d, %d, %d, %d\r\n", wb.g_x, wb.g_y, wb.g_w, wb.g_h);

                    wind_get_grect(whandle, WF_FIRSTXYWH, &wr);
                    do
                    {
                        dial[0].ob_x = ww.g_x;
                        dial[0].ob_y = ww.g_y;
                        objc_draw_grect(dial, ROOT, MAX_DEPTH, &wr);
                        wind_get_grect(whandle, WF_NEXTXYWH, &wr);
                    } while (wr.g_w != 0 && wr.g_h != 0);
                    break;

                case WM_CLOSED:
                    wind_close(whandle);
                    wind_delete(whandle);

                    if (_app)
                        quit = true;
            }
        }
    } while (!quit);

    return appl_exit();
}
