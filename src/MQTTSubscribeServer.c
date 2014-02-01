/*******************************************************************************
 * Copyright (c) 2014 IBM Corp.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Ian Craggs - initial API and implementation and/or initial documentation
 *******************************************************************************/

#include "MQTTPacket.h"
#include "StackTrace.h"

#include <string.h>

int MQTTDeserialize_subscribe(int* dup, int* msgid, int max_count, int* count, MQTTString topicStrings[], int reqQos[], char* buf, int len)
{
	MQTTHeader header;
	char* curdata = buf;
	char* enddata = NULL;
	int rc = -1;
	int mylen = 0;

	FUNC_ENTRY;
	header.byte = readChar(&curdata);
	*dup = header.bits.dup;

	curdata += (rc = MQTTPacket_decodeBuf(curdata, &mylen)); /* read remaining length */
	enddata = curdata + mylen;

	*msgid = readInt(&curdata);

	*count = 0;
	while (curdata < enddata)
	{
		if (!readMQTTLenString(&topicStrings[*count], &curdata, enddata))
			goto exit;
		if (curdata >= enddata) /* do we have enough data to read the req_qos version byte? */
			goto exit;
		reqQos[*count] = readChar(&curdata);
		(*count)++;
	}

	rc = 1;
exit:
	FUNC_EXIT_RC(rc);
	return rc;
}


int MQTTSerialize_suback(char* buf, int buflen, int msgid, int count, int* granted_qos)
{
	MQTTHeader header;
	int rc = -1;
	char *ptr = buf;
	int i;

	FUNC_ENTRY;
	if (buflen < 2 + count)
	{
		rc = MQTTPACKET_BUFFER_TOO_SHORT;
		goto exit;
	}
	header.byte = 0;
	header.bits.type = SUBACK;
	writeChar(&ptr, header.byte); /* write header */

	ptr += MQTTPacket_encode(ptr, 2 + count); /* write remaining length */

	writeInt(&ptr, msgid);

	for (i = 0; i < count; ++i)
		writeChar(&ptr, granted_qos[i]);

	rc = ptr - buf;
exit:
	FUNC_EXIT_RC(rc);
	return rc;
}

