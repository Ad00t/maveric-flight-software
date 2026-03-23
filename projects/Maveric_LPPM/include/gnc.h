// GNC Planner Functions
// Function Declaration
uint8_t Mode MTQ_STAT_Parse_Mode(uint8_t MTQ_STAT);
bool SUN MTQ_STAT_Parse_SUN(uint8_t MTQ_STAT);
bool TUMB MTQ_STAT_Parse_TUMB(uint8_t MTQ_STAT);
void MTQ_GNC_Planner(MTQ_Planner_State_t* state, mtq_s* mtq, uint32_t MTQ_STAT, float MTQ_RATE);