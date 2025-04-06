# cmpe230

Proje Özeti — Witcher Tracker Yorumlayıcısı (Mimari Bakış)
✅ 3 Girdi Türü
Sentence: Geralt bir aksiyon yapar (loot, trade, brew, learn, encounter)

Question: Bilgi sorgular (inventory, bestiary, alchemy)

Exit: Programı sonlandırır

⚙️ Aksiyonlar (Sentence)
Komut	Açıklama
loot	Malzeme toplar → envantere eklenir
trade	Trofe → malzeme takası
brew	Malzemeyle iksir üretme
learn	İksir formülü ya da canavara etkili bilgi öğrenme
encounter	Canavarla karşılaşma (etkili iksir/sign varsa döver, yoksa kaçar)
❓ Sorgular (Question)
Total ingredient/potion/trophy ? → hepsini listeler

Total ingredient Rebis ? → spesifik nesne miktarı

What is in Black Blood ? → iksir içeriği

What is effective against Harpy ? → etkili iksir/sign listesi

💬 Dil Kuralları
Büyük-küçük harfe duyarlı

Sayı zorunlu (loot/trade)

Noktalama, spacing, gramer %100 kontrol edilecek

Kurallara uymayan her şey → INVALID