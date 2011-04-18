int PSYC_renderHeader(struct PSYC_Buffers* pbuf,
                      const uint8_t* name, const size_t nlength,
                      const uint8_t* value, const size_t vlength,
                      const uchar flags, const uchar modifier);

int PSYC_renderBody(struct PSYC_Buffers* pbuf,
                    const uint8_t* method, const size_t mlength,
                    const uint8_t* data, const size_t dlength,
                    const uchar flags);

int PSYC_doneRender(struct PSYC_Buffers* pbuf,
                    uint8_t** buf, size_t* written);

