/* Stack rules inclusion.*/
INCLUDE ../rules_stacks.ld

/* Code rules inclusion.*/
INCLUDE ../rules_code.ld

/* Data rules inclusion.*/
INCLUDE ../rules_data.ld

/* Memory rules inclusion.*/
INCLUDE ../rules_memory.ld

SECTIONS
{
    /***************************************
     *  Here is place to read and write to *
     ***************************************/
    MyFlash1 0x0800F400 :
    {
        *(MyFlash)
    } > flash0
}