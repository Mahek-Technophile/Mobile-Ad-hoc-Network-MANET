// CRC-16-CCITT and Checksum math corresponding to include/ErrorControl.hpp
export function computeCRC16(data: string): number {
  let crc = 0xffff;
  for (let i = 0; i < data.length; i++) {
    const byte = data.charCodeAt(i) & 0xff;
    crc ^= byte << 8;
    for (let bit = 0; bit < 8; bit++) {
      if ((crc & 0x8000) !== 0) {
        crc = ((crc << 1) ^ 0x1021) & 0xffff;
      } else {
        crc = (crc << 1) & 0xffff;
      }
    }
  }
  return crc;
}

export function computeInternetChecksum(data: string): number {
  let sum = 0;
  let i = 0;
  while (i + 1 < data.length) {
    const word = (data.charCodeAt(i) << 8) | data.charCodeAt(i + 1);
    sum += word;
    i += 2;
  }
  if (i < data.length) {
    sum += data.charCodeAt(i) << 8;
  }
  while (sum >> 16) {
    sum = (sum & 0xffff) + (sum >> 16);
  }
  return ~sum & 0xffff;
}

export function encodeHamming74(nibble: number): number {
  const d1 = (nibble >> 0) & 1;
  const d2 = (nibble >> 1) & 1;
  const d3 = (nibble >> 2) & 1;
  const d4 = (nibble >> 3) & 1;

  const p1 = d1 ^ d2 ^ d4;
  const p2 = d1 ^ d3 ^ d4;
  const p3 = d2 ^ d3 ^ d4;

  return (
    (p1 << 0) |
    (p2 << 1) |
    (d1 << 2) |
    (p3 << 3) |
    (d2 << 4) |
    (d3 << 5) |
    (d4 << 6)
  );
}

export function decodeHamming74(code: number): {
  data: number;
  hadError: boolean;
  wasCorrected: boolean;
  syndrome: number;
} {
  const b1 = (code >> 0) & 1;
  const b2 = (code >> 1) & 1;
  const b3 = (code >> 2) & 1;
  const b4 = (code >> 3) & 1;
  const b5 = (code >> 4) & 1;
  const b6 = (code >> 5) & 1;
  const b7 = (code >> 6) & 1;

  const s1 = b1 ^ b3 ^ b5 ^ b7;
  const s2 = b2 ^ b3 ^ b6 ^ b7;
  const s3 = b4 ^ b5 ^ b6 ^ b7;

  const syndrome = (s3 << 2) | (s2 << 1) | (s1 << 0);
  const hadError = syndrome !== 0;
  let correctedCode = code;
  let wasCorrected = false;

  if (hadError && syndrome <= 7) {
    correctedCode ^= 1 << (syndrome - 1);
    wasCorrected = true;
  }

  const d1 = (correctedCode >> 2) & 1;
  const d2 = (correctedCode >> 4) & 1;
  const d3 = (correctedCode >> 5) & 1;
  const d4 = (correctedCode >> 6) & 1;

  return {
    data: (d4 << 3) | (d3 << 2) | (d2 << 1) | d1,
    hadError,
    wasCorrected,
    syndrome,
  };
}

export function distance2D(a: { x: number; y: number }, b: { x: number; y: number }): number {
  const dx = a.x - b.x;
  const dy = a.y - b.y;
  return Math.sqrt(dx * dx + dy * dy);
}
