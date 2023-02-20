/*****************************************
* This is a library for the SI1145 ALS, IR, UV and PS Sensor Module
*
* You'll find an example which should enable you to use the library. 
*
* You are free to use it, change it or build on it. In case you like 
* it, it would be cool if you give it a star.
* 
* If you find bugs, please inform me!
* 
* Written by Wolfgang (Wolle) Ewald
* https://wolles-elektronikkiste.de
*
*******************************************/

#include "SI1145_WE.h"
#include "../i2c/i2c.h"

void SI1145_WE_init(){ 
	i2cAddress = 0x60;
    /* sets default parameters */
        
    resetSI1145();
    
    /* UV default calibration values */
    setRegister(SI1145_REG_UCOEFF0, 0x29);
    setRegister(SI1145_REG_UCOEFF1, 0x89);
    setRegister(SI1145_REG_UCOEFF2, 0x02);
    setRegister(SI1145_REG_UCOEFF3, 0x00);
    
    /* Interrupt Settings */
    enableInterrupt(PSALS_INT);
    
    /* pause between measurements =  31.25 µs x rate */
    setMeasurementRate(0x00FF);
    
    /* set LED Current, 22.4 mA */
    setLEDCurrent(3); 
    
    /* prox sensor 1 uses LED1 */
    setParameter(SI1145_PARA_PSLED12_SELECT, 0x01);
    
    /* PS Photodiode Selection */
    selectPsDiode(LARGE_DIODE);
    
    /* Pulse Width Selection of IR LED pulse; pulse width = pulse width * 2^PS_ADC_Gain 
        No value > 5 shall be taken */
    setPsAdcGain(0);
    disableHighSignalPsRange();
    
    /* ALS-IR Photodiode Selection */
    selectIrDiode(SMALL_DIODE);
    
    /* Integration time for ALS-VIS measurements = integration time * 2^ALS_VIS_ADC_GAIN 
        No value > 7 (factor 128) shall be taken */
    setAlsVisAdcGain(0);
    disableHighSignalVisRange();
    
    /* Integration time for ALS-IR measurements = integration time * 2^ALS_IR_ADC_GAIN 
        No value > 7 (factor 128) shall be taken */
    setAlsIrAdcGain(0);
    disableHighSignalIrRange();
}

void resetSI1145(){
    setRegister(SI1145_REG_COMMAND, SI1145_RESET);
    _delay_ms(10);
    setHwKey();
    _delay_ms(10);
}

void setI2CAddress(uint8_t addr){
	i2cAddress = addr;
    setParameter(SI1145_PARA_I2C_ADDR, addr); _delay_ms(10);
    setRegister(SI1145_REG_COMMAND, SI1145_BUSADDR); _delay_ms(10);
}

void enableMeasurements(SI1145MeasureType t, SI1145MeasureMode m){
    deviceMeasureMode = m;
    deviceMeasureType = t;
    uint8_t chListValue = 0;
    uint8_t typeAndMode = 0;
    
    switch(t){
        case PS_TYPE:
            chListValue = 0b00000001;
            break;
        case ALS_TYPE:
            chListValue = 0b00110000;   // IR wird immer mit ALS zusammen aktiviert
            break;
        case PSALS_TYPE:
            chListValue = 0b00110001;
            break;
        case ALSUV_TYPE:
            chListValue = 0b10110000;
            break;
        case PSALSUV_TYPE:
            chListValue = 0b10110001;
            break;
    }
    typeAndMode = m | t; 
    setParameter(SI1145_PARA_CHLIST, chListValue);
    setRegister(SI1145_REG_COMMAND, typeAndMode);
    _delay_ms(10);
}

void startSingleMeasurement(){
    uint8_t typeAndMode = deviceMeasureMode | deviceMeasureType;
    setRegister(SI1145_REG_COMMAND, typeAndMode);
}

void enableInterrupt(SI1145IntType t){
    setRegister(SI1145_REG_INT_CFG, 1);
    setRegister(SI1145_REG_IRQEN, t);
}

void disableAllInterrupts(){
    setRegister(SI1145_REG_INT_CFG, 0);
}

void setMeasurementRate(uint16_t rate){
    setRegister16bit(SI1145_REG_MEASRATE0, rate);
}

void setHwKey(){
    setRegister(SI1145_REG_HW_KEY, 0x17);
}

void setLEDCurrent(uint8_t curr){
    setRegister(SI1145_REG_PSLED21, curr);
}

void selectPsDiode(SI1145_DIODE diode){
    setParameter(SI1145_PARA_PS1_ADCMUX, diode);
}

void selectIrDiode(SI1145_DIODE diode){
    setParameter(SI1145_PARA_ALS_IR_ADCMUX, diode);
}

void enableHighResolutionPs(){
    setParameter(SI1145_PARA_PS_ENCODING, 0b0001000);
}

void disableHighResolutionPs(){
    setParameter(SI1145_PARA_PS_ENCODING, 0b0000000);
}

void enableHighResolutionVis(){
    uint8_t ALSEncodingStatus = getParameter(SI1145_PARA_ALS_ENCODING);
    ALSEncodingStatus |= 0b00010000;
    setParameter(SI1145_PARA_ALS_ENCODING, ALSEncodingStatus);
}

void disableHighResolutionVis(){
    uint8_t ALSEncodingStatus = getParameter(SI1145_PARA_ALS_ENCODING);
    ALSEncodingStatus &= 0b11101111;
    setParameter(SI1145_PARA_ALS_ENCODING, 0b0000000);
}

void enableHighResolutionIr(){
    uint8_t ALSEncodingStatus = getParameter(SI1145_PARA_ALS_ENCODING);
    ALSEncodingStatus |= 0b00100000;
    setParameter(SI1145_PARA_ALS_ENCODING, ALSEncodingStatus);
}

void disableHighResolutionIr(){
    uint8_t ALSEncodingStatus = getParameter(SI1145_PARA_ALS_ENCODING);
    ALSEncodingStatus &= 0b11011111;
    setParameter(SI1145_PARA_ALS_ENCODING, ALSEncodingStatus);
}

void setPsAdcGain(unsigned char psAdcGain){      /* psAdcGain <= 5! */
    setParameter(SI1145_PARA_PS_ADC_GAIN, psAdcGain);
    /* data sheet recommendation: psAdcRec = one's complement of psAdcGain */
    unsigned char psAdcRec = ((~psAdcGain) & 0b00000111) << 4; 
    setParameter(SI1145_PARA_PS_ADCOUNTER, psAdcRec);
}

void enableHighSignalPsRange(){
    setParameter(SI1145_PARA_PS_ADC_MISC, 0x24); // change of PS_ADC_MODE not implemented
}
    
void disableHighSignalPsRange(){
    setParameter(SI1145_PARA_PS_ADC_MISC, 0x04); // change of PS_ADC_MODE not implemented
}

void setAlsVisAdcGain(unsigned char alsVisAdcGain){    /* alsVisAdcGain <= 7 */
    setParameter(SI1145_PARA_ALS_VIS_ADC_GAIN, alsVisAdcGain);
    /* data sheet recommendation: visAdcRec = one's complement of alsVisAdcGain */
    unsigned char visAdcRec = ((~alsVisAdcGain) & 0b00000111) << 4;
    setParameter(SI1145_PARA_ALS_VIS_ADC_COUNTER, visAdcRec);
}

void enableHighSignalVisRange(){
    setParameter(SI1145_PARA_ALS_VIS_ADC_MISC, 0x20); // change of PS_ADC_MODE not implemented
}
    
void disableHighSignalVisRange(){
    setParameter(SI1145_PARA_ALS_VIS_ADC_MISC, 0x00); // change of PS_ADC_MODE not implemented
}

void setAlsIrAdcGain(unsigned char alsIrAdcGain){      /* irAdcGain <= 7 */
    setParameter(SI1145_PARA_ALS_IR_ADC_GAIN, alsIrAdcGain);
    /* data sheet recommendation: irAdcRec = one's complement of alsIrAdcGain */
    unsigned char irAdcRec = ((~alsIrAdcGain) & 0b00000111) << 4; 
    setParameter(SI1145_PARA_ALS_IR_ADC_COUNTER, irAdcRec);
}

void enableHighSignalIrRange(){
    setParameter(SI1145_PARA_ALS_IR_ADC_MISC, 0x20); 
}
    
void disableHighSignalIrRange(){
    setParameter(SI1145_PARA_ALS_IR_ADC_MISC, 0x00); 
}

uint16_t getAlsVisData(){
    return getRegister16bit(SI1145_REG_ALS_VIS_DATA0);
}

uint16_t getAlsIrData(){
    return getRegister16bit(SI1145_REG_ALS_IR_DATA0);
}

uint16_t getPsData(){
    return getRegister16bit(SI1145_REG_PS1_DATA0);
}

float getUvIndex(){
    float uvi = getRegister16bit(SI1145_REG_UV_INDEX0)/100.0;
    return uvi;
}

uint8_t getFailureMode(){
    return getRegister(SI1145_REG_RESPONSE);
}

void clearFailure(){
    setRegister(SI1145_REG_COMMAND, SI1145_NOP);
}

void clearAllInterrupts(){
    setRegister(SI1145_REG_IRQ_STAT, 0xFF);
}

void clearAlsInterrupt(){
    setRegister(SI1145_REG_IRQ_STAT, ALS_INT);
}

void clearPsInterrupt(){
    setRegister(SI1145_REG_IRQ_STAT, PS_INT);
}

void clearCmdInterrupt(){
    setRegister(SI1145_REG_IRQ_STAT, CMD_INT);
}

uint8_t getInterruptStatus(){
    return getRegister(SI1145_REG_IRQ_STAT);
}


/************************************************ 
    private functions
*************************************************/

uint8_t setParameter(uint8_t param, uint8_t cmd){
    param |= SI1145_PARAM_SET;
    setRegister(SI1145_REG_PARAM_WR, cmd);
    setRegister(SI1145_REG_COMMAND, param);
    return getRegister(SI1145_REG_PARAM_RD);
}

uint8_t getParameter(uint8_t param){
    param |= SI1145_PARAM_QUERY;
    setRegister(SI1145_REG_COMMAND, param);
    return getRegister(SI1145_REG_PARAM_RD);
}


uint8_t getRegister(uint8_t registerAddr)
{
    uint8_t data;
	
	I2C_Start();
	I2C_Write(0xC1);
	I2C_Write(registerAddr); /////
	
	//I2C_Write(i2cAddress);
    //_wire->endTransmission(false); 
    //_wire->requestFrom(i2cAddress , static_cast<uint8_t>(1));
    data = I2C_Read(0); 
	I2C_Stop();
	
    return data;
}

uint16_t getRegister16bit(uint8_t registerAddr)
{
    uint8_t data_low;
    uint8_t data_high;
    uint16_t data;

	I2C_Start();
	I2C_Write(0xC0);
	I2C_Write(registerAddr);
	//I2C_Stop(); 
    
    //_wire->endTransmission(false); 
	
	
    //_wire->requestFrom(i2cAddress, static_cast<uint8_t>(2));
	I2C_Start();
	I2C_Write(0xC1);
    data_low = I2C_Read(1);
    data_high = I2C_Read(0); 
    data = (data_high << 8)|data_low;
	I2C_Stop();
	
    return data;
}

void setRegister(uint8_t registerAddr, uint8_t data)
{
	I2C_Start();  
	I2C_Write(0xC0);
	I2C_Write(registerAddr);
	//I2C_Stop();
	
	//I2C_Start();
	//I2C_Write(i2cAddress);
	I2C_Write(data);
	I2C_Stop(); 
}

void setRegister16bit(uint8_t registerAddr, uint16_t data)
{
    I2C_Start();
    I2C_Write(0xC0);
    I2C_Write(registerAddr);
	//I2C_Stop(); 
	
	//I2C_Start();
	//I2C_Write(i2cAddress);
    uint8_t temp = data & 0xff;
    I2C_Write(temp); 
    temp = (data >> 8) & 0xff;
    I2C_Write(temp); 
    I2C_Stop(); 
}
    


