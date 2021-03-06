#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <gem.h>

#define DEBUG
#ifdef DEBUG
#include "natfeats.h"
#define dbg(format, arg...) do { nf_printf("DEBUG: (%s):" format, __FUNCTION__, ##arg); } while (0)
#define out(format, arg...) do { nf_printf("" format, ##arg); } while (0)
#else
#define dbg(format, arg...) do { ; } while (0)
#endif /* DEBUG */

static short vdi_handle;
static short g_wc, g_hc, g_wb, g_hb;

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

struct uparm
{

} uparms =
{

};

int main(void)
{
    short ap_id;
    static short whandle = -1;
    GRECT wb;
    GRECT ww;
    short evnt;
    bool quit = false;

#define ROOTOBJ         0
#define CLICK_ME        1
#define SHOW_CLICK      2
#define PART_FRAME      3
#define PART_ONE        4
#define PART_TWO        5
#define PART_ONE_KNOB   6
#define PART_TWO_KNOB   7

    TEDINFO part1_but =
    {
        .te_ptext = "1000",
        .te_ptmplt = NULL,
        .te_pvalid = "9999",
        .te_color = 1 << 8 | 1 << 12,
        .te_font = 5,
        .te_fontid = 0,
        .te_just = TE_CNTR,
        .te_thickness = 1
    };

    TEDINFO part2_but =
    {
        .te_ptext = "1001",
        .te_ptmplt = NULL,
        .te_pvalid = "9999",
        .te_color = 1 << 8 | 1 << 12,
        .te_font = 5,
        .te_fontid = 0,
        .te_just = TE_CNTR,
        .te_thickness = 1
    };

    OBJECT dial[] =
    {
        /* a dialog object frame */
        {
            /* 0 */
            .ob_next = NIL,
            .ob_head = CLICK_ME,
            .ob_tail = PART_FRAME,
            .ob_type = G_BOX,
            .ob_flags = OF_FL3DBAK,
            .ob_state = OS_NORMAL,
            .ob_spec = { 0L },
            .ob_x = 0,
            .ob_y = 0,
            .ob_width = 210,
            .ob_height = 270
        },
        /* with one single button in it */
        {
            /* 1 */
            .ob_next = SHOW_CLICK,
            .ob_head = NIL,
            .ob_tail = NIL,
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
            /* 2 */
            .ob_next = PART_FRAME,
            .ob_head = NIL,
            .ob_tail = NIL,
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
            /* 3 */
            .ob_next = ROOTOBJ,
            .ob_head = PART_ONE,
            .ob_tail = PART_TWO_KNOB,
            .ob_type = G_BOX,
            .ob_flags = OF_NONE | OF_FL3DIND,
            .ob_state = OS_NORMAL,
            .ob_spec.obspec = { 0L },
            .ob_x = 20,
            .ob_y = 80,
            .ob_width = 100,
            .ob_height = 150
        },
        {
            /* 4 */
            .ob_next = PART_TWO,
            .ob_head = NIL,
            .ob_tail = NIL,
            .ob_type = G_BOXTEXT,
            .ob_flags = OF_SELECTABLE,
            .ob_state = OS_NORMAL,
            .ob_spec.tedinfo = &part1_but,
            .ob_x = 0,
            .ob_y = 0,
            .ob_width = 100,
            .ob_height = 75
        },
        {
            /* 5 */
            .ob_next = PART_ONE_KNOB,
            .ob_head = NIL,
            .ob_tail = NIL,
            .ob_type = G_BOXTEXT,
            .ob_flags = OF_SELECTABLE,
            .ob_state = OS_NORMAL,
            .ob_spec.tedinfo = &part2_but,
            .ob_x = 0,
            .ob_y = 75,
            .ob_width = 100,
            .ob_height = 75
        },
        {
            /* 6 */
            .ob_next = PART_TWO_KNOB,
            .ob_head = NIL,
            .ob_tail = NIL,
            .ob_type = G_BOX,
            .ob_flags = OF_SELECTABLE | OF_TOUCHEXIT | OF_FL3DACT,
            .ob_spec.obspec = { .interiorcol = G_RED, .fillpattern = IP_SOLID },
            .ob_x = (short)(dial[PART_ONE].ob_x + dial[PART_TWO].ob_width - 20),
            .ob_y = (short)(dial[PART_ONE].ob_y - 3),
            .ob_width = 5,
            .ob_height = 5
        },
        {
            /* 7 */
            .ob_next = PART_FRAME,
            .ob_head = NIL,
            .ob_tail = NIL,
            .ob_type = G_BOX,
            .ob_flags = OF_SELECTABLE | OF_TOUCHEXIT | OF_FL3DACT,
            .ob_spec = { 0L },
            .ob_x = (short) (dial[PART_TWO].ob_x + dial[PART_TWO].ob_width - 20),
            .ob_y = (short) (dial[PART_TWO].ob_y - 3),
            .ob_width = 5,
            .ob_height = 5
        }

    };

    ap_id = appl_init();
    vdi_handle = graf_handle(&g_wc, &g_hc, &g_wb, &g_hb);

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
            if ((dial[ob].ob_flags & OF_SELECTABLE) && (! (dial[ob].ob_flags == OF_TOUCHEXIT)))
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

            if (ob == PART_TWO_KNOB)
            {
                short perc;

                dbg("touchexit object selected\r\n");
                graf_mouse(FLAT_HAND, NULL);
                perc = graf_slidebox(dial, 3, 7, 1);
                graf_mouse(ARROW, NULL);
                dial[PART_TWO_KNOB].ob_state &= ~OS_SELECTED;
                dbg("perc=%d\r\n", perc);

                dial[PART_ONE].ob_height = dial[PART_FRAME].ob_height * perc / 1000L;
                dial[PART_TWO].ob_height = dial[PART_FRAME].ob_height * (1000 - perc) / 1000L;
                dial[PART_TWO].ob_y = dial[PART_ONE].ob_height;
                sprintf(dial[PART_ONE].ob_spec.tedinfo->te_ptext, "%d", perc);
                sprintf(dial[PART_TWO].ob_spec.tedinfo->te_ptext, "%d", 1000 - perc);

                dial[PART_TWO_KNOB].ob_y = dial[PART_TWO].ob_y - 3;

                appl_write(ap_id, sizeof(msg), msg);
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
